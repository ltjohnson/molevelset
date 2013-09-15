molevelset <- function(X, Y, gamma, k.max, delta=0.05, rho=0.05) {
    UseMethod("molevelset")
}

molevelset.default <- function(X, Y, gamma, k.max, delta=0.05, rho=0.05) {
    stop("X has unsupported class ", class(X), ".")
}

molevelset.formula <- function(X, Y, gamma, k.max=3, delta=0.05,
                               rho=0.05) {
  cl <- match.call()
  m <- model.frame(X, Y)
 
  terms <- attr(m, "terms")
  if (any(attr(terms, "order") > 1)) {
    stop("molevelset cannot handle interaction terms.")
  }
  Y <- model.extract(m, "response")
  if (NCOL(Y) > 1) {
    stop("molevelset cannot handle more than one response.")
  }
  if (!is.numeric(Y)) {
    stop("molevelset can only handle numeric response variables.")
  }
  Y <- as.numeric(Y)

  X <- m[as.character(attr(terms, "term.labels"))]
  if (!all(sapply(X, is.numeric))) {
    stop("molevelset can only handle numeric predictors.")
  }

  X <- as.matrix(sapply(X, as.numeric))

  le <- molevelset.matrix(X, Y, gamma, k.max=k.max, delta=delta, rho=rho)

  le$method      <- "formula"
  le$X           <- NULL
  le$Y           <- NULL
  le$model.frame <- m
  le$call        <- cl
  class(le)      <- "molevelset"
    
  return(le)
}

molevelset.matrix <- function(X, Y, gamma, k.max=3, delta=0.05, rho=0.05) {
  stopifnot(is.matrix(X), is.vector(Y))
  cl <- match.call()

  transform <- NULL
  if (any(X < 0 | X > 1)) {
      transformed <- transform.X(X, k.max)
      transform <- transformed$transform
      X <- transformed$X
  }

  le <- .Call("estimate_levelset", X, Y, as.integer(k.max), gamma, delta, rho,
              PACKAGE="molevelset")

  inset_checks <-t(sapply(le$inset_boxes, function(b) t(b$box)))
  if (!NROW(inset_checks) || !NCOL(inset_checks)) {
      inset_checks <- matrix(0, ncol=NCOL(X) * 2, nrow=0)
  }
  n.x <- ncol(X)
  le$inset_checks <-
    lapply(seq_len(n.x), function(i) inset_checks[, c(i, i + n.x), drop=FALSE])
  names(le$inset_checks) <- colnames(X)

  le$X.names <- colnames(X)
  if (is.null(colnames(X))) {
    le$X.names <- seq_len(n.x)
  }
  
  le$X      <- X
  le$Y      <- Y
  le$k.max  <- k.max
  le$gamma  <- gamma
  le$delta  <- delta
  le$rho    <- rho
  le$method <- "matrix"
  le$call   <- cl
  class(le) <- "molevelset"
  
  return(le)
}

in.molevelset <- function(levelset.estimate, X) {
  switch(levelset.estimate$method,
         formula=in.molevelset.formula(X, levelset.estimate),
         matrix=in.molevelset.matrix(X, levelset.estimate))
}

.in.molevelset.inner.formula <- function(x, bounds) {
  tmp <- lapply(names(bounds), function(nm)
                x[[nm]] > bounds[[nm]][, 1] &
                x[[nm]] <= bounds[[nm]][, 2])
  any(do.call("&", tmp))
}

.in.molevelset.inner.matrix <- function(x, bounds) {
  tmp <- lapply(seq_len(ncol(x)), function(i)
                x[, i] > bounds[[i]][, 1] &
                x[, i] <= bounds[[i]][, 2])
  any(do.call("&", tmp))
}

in.molevelset.formula <- function(X, levelset.estimate) {
  X.names <- levelset.estimate$X.names
  if (!is.data.frame(X) || any(!(X.names %in% names(X)))) {
    stop("X must be a data.frame containing the same columns used in the ",
         "formula.")
  }
  in.box <-
    sapply(seq_len(nrow(X)), function(i)
           .in.molevelset.inner.formula(X[i, , drop=FALSE],
                                        levelset.estimate$inset_checks))
  return(in.box)
}

in.molevelset.matrix <- function(X, levelset.estimate) {
  X.names <- levelset.estimate$X.names
  if (!is.matrix(X) ||
      (is.null(colnames(X)) && ncol(X) != length(X.names)) ||
      (!is.null(colnames(X)) && all(X.names %in% colnames(X)))) {
    stop("X must be a matrix with the same columns as used in X",
         " passed to molevelset.matrix.")
  }
  in.box <-
    sapply(seq_len(nrow(X)), function(i)
           .in.molevelset.inner.matrix(X[i, , drop=FALSE],
                                       levelset.estimate$inset_checks))
  return(in.box)
}

transform.X <- function(X, k.max) {
    # pick delta for the transformation, we'll preserve aspect ratio.
    original.range <- max(apply(X, 2, function(col) max(col) - min(col)))
    target.range <- 1 - 1 / 2^(k.max + 2)
    original.center <- colMeans(X)
    transform <- list(original.range=original.range,
                      target.range=target.range,
                      original.center=original.center)
    for (i in seq_len(NCOL(X))) {
        X[, i] <- 1 / 2 + target.range * (X[, i] - original.center[i]) /
            original.range
    }
    return(list(transform=transform, X=X))
}
