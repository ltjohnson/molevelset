molevelset <- function(X, Y, gamma, k.max, delta=0.05, rho=0.05) {
  cl <- match.call()

  if (class(X) == "formula") {
    levelset.estimate <-
      molevelset.formula(formula=X, data=Y, gamma=gamma, k.max=k.max,
                         delta=delta, rho=rho)
  } else if (is.matrix(X)) {
    levelset.estimate <-
      molevelset.matrix(X=X, Y=Y, gamma=gamma, k.max=k.max, delta=delta,
                        rho=rho)
  }

  levelset.estimate$call <- cl
  
  return(levelset.estimate)
}

molevelset.formula <- function(formula, data, gamma, k.max=3, delta=0.05,
                               rho=0.05) {
  cl <- match.call()
  m <- model.frame(formula, data)
 
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
  if (any(sapply(X.orig, is.numeric) == FALSE)) {
    stop("molevelset can only handle numeric predictors.")
  }

  X <- as.matrix(sapply(X, as.numeric))

  le <- molevelset.matrix(X, Y, gamma, k.max=k.max, delta=delta, rho=rho)

  le$method      <- "formula"
  le$call        <- cl
  le$X           <- NULL
  le$Y           <- NULL
  le$model.frame <- m

  return(le)
}

molevelset.matrix <- function(X, Y, gamma, k.max=3, delta=0.05, rho=0.05) {
  stopifnot(is.matrix(X), is.vector(Y))
  cl <- match.call()
  le <- .Call("estimate_levelset", X, Y, as.integer(k.max), gamma, delta, rho)

  inset_checks <-t(sapply(le$inset_boxes, function(b) t(b$box)))
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
  le$call   <- cl
  le$method <- "matrix"
  
  return(le)
}

in.levelset <- function(X, levelset.estimate) {
  switch(levelset.estimate$method,
         formula=in.levelset.formula(X, levelset.estimate),
         matrix=in.levelset.matrix(X, levelset.estimate))
}

.in.levelset.inner.formula <- function(x, bounds) {
  tmp <- lapply(names(bounds), function(nm)
                x[[nm]] > bounds[[nm]][, 1] &
                x[[nm]] <= bounds[[nm]][, 2])
  any(do.call("&", tmp))
}

.in.levelset.inner.matrix <- function(x, bounds) {
  tmp <- lapply(seq_len(ncol(x)), function(i)
                x[, i] > bounds[[i]][, 1] &
                x[, i] <= bounds[[i]][, 2])
  any(do.call("&", tmp))
}

in.levelset.formula <- function(X, levelset.estimate) {
  X.names <- levelset.estimate$X.names
  if (!is.data.frame(X) || any(!(X.names %in% names(X)))) {
    stop("X must be a data.frame with containing at least the",
         " same columns used in the formula.")
  }
  in.box <-
    sapply(seq_len(nrow(X)), function(i)
           .in.levelset.inner.formula(X[i, , drop=FALSE],
                                      levelset.estimate$inset_checks))
  return(in.box)
}

in.levelset.matrix <- function(X, levelset.estimate) {
  X.names <- levelset.estimate$X.names
  if (!is.matrix(X) ||
      (is.null(colnames(X)) && ncol(X) != length(X.names)) ||
      (!is.null(colnames(X)) && all(X.names %in% colnames(X)))) {
    stop("X must be a matrix with the same columns as used in X",
         " passed to molevelset.matrix.")
  }
  in.box <-
    sapply(seq_len(nrow(X)), function(i)
           .in.levelset.inner.matrix(X[i, , drop=FALSE],
                                     levelset.estimate$inset_checks))
  return(in.box)
}
