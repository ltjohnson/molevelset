library(molevelset)

TestEmptyLevelSet <- function() {
    # Make sure empty and full levelsets are handled properly.
    # Create a grid of points that cover all boxes in the levelset.
    k.max <- 1
    x <- 1 / (2 * 2^k.max) + (seq_len(2^k.max) - 1) / 2^k.max
    X <- as.matrix(expand.grid(x, x))
    n <- NROW(X)
    
    Y <- rep(0, n)
    le <- molevelset(X, Y, k.max=k.max, gamma=0.50, rho=10)
    stopifnot(length(le$inset_boxes) == 0,
              length(le$non_inset_boxes) == 1,
              identical(sort(le$non_inset_boxes[[1]]$i),
                        seq_len(NROW(X))))
    
    Y <- rep(1, n)
    le <- molevelset(X, Y, k.max=k.max, gamma=0.50, rho=10)
    stopifnot(length(le$inset_boxes) == 1,
              length(le$non_inset_boxes) == 0,
              identical(sort(le$inset_boxes[[1]]$i),
                        seq_len(NROW(X))))
    
    return(TRUE)
}

TestClassSetProperly <- function() {
    k.max <- 3
    X <- matrix(runif(10), ncol=2)
    Y <- rep(1, nrow(X))
    le <- molevelset(X, Y, 0.5, k.max)
    stopifnot(class(le) == "molevelset")

    X <- data.frame(X=X[, 1], Y=X[, 2], Z=Y)
    le <- molevelset(Z ~ X + Y, X, 0.5, k.max)
    stopifnot(class(le) == "molevelset")

    return(TRUE)
}

TestTotalCost <- function() {
    X <- matrix(runif(10), ncol=2)
    Y <- rep(1, nrow(X))
    le <- molevelset(X, Y, gamma=0.5, k.max=0, rho=0)
    stopifnot(identical(le$total_cost,
                        sum(0.5 - Y) / 2 / (max(abs(Y)) + 1)),
              identical(sort(unlist(lapply(le$inset_boxes, "[[", "i"))),
                        seq_len(NROW(X))))

    Y <- c(1, 1, 1, 0, 0)
    le <- molevelset(X, Y, gamma=0.5, k.max=0, rho=0)
    stopifnot(identical(le$total_cost,
                        sum(0.5 - Y) / 2 / (max(abs(Y)) + 1)),
              identical(sort(unlist(lapply(le$inset_boxes, "[[", "i"))),
                        seq_len(NROW(X))))

    return(TRUE)
}

TestIValues <- function() {
    X <- matrix(0.234, ncol=2, nrow=5)
    Y <- c(1, 1, 1, 0, 0)
    le <- molevelset(X, Y, gamma=0.5, k.max=0, rho=0)

    stopifnot(identical(le$num_boxes, 1),
              identical(sort(unlist(lapply(le$inset_boxes, "[[", "i"))),
                        seq_len(NROW(X))),
              identical(le$total_cost, sum(0.5 - Y) / 2 / (max(abs(Y)) + 1)))
    return(TRUE)
}

TestInsetMatrix <- function() {
    X <- cbind(c(0.25, 0.25, 0.75, 0.75),
               c(0.25, 0.75, 0.75, 0.25))
    Y <- c(0, 1, 1, 0)
    le <- molevelset(X, Y, gamma=0.5, k.max=1, rho=0)

    X.test <- cbind(c(0.33, 0.4, 0.7, 0.9),
                    c(0.1, 0.7, 0.66, 0.2))
    expected <- c(FALSE, TRUE, TRUE, FALSE)
    stopifnot(identical(expected, in.molevelset(le, X.test)))

    return(TRUE)
}

TestInsetFormula <- function() {
    X <- data.frame(X=c(0.25, 0.25, 0.75, 0.75),
                    Y=c(0.25, 0.75, 0.75, 0.25),
                    Z=c(0, 1, 1, 0))
    le <- molevelset(Z ~ X + Y, X, gamma=0.5, k.max=1, rho=0)

    X.test <- data.frame(X=c(0.33, 0.4, 0.7, 0.9),
                         Y=c(0.1, 0.7, 0.66, 0.2))
    expected <- c(FALSE, TRUE, TRUE, FALSE)
    stopifnot(identical(expected, in.molevelset(le, X.test)))

    return(TRUE)
}

test.names <- ls(pattern="^Test.*")
test.functions <- lapply(test.names, get)
test.i <- which(sapply(test.functions, class) == "function")

for (i in test.i) {
    cat(test.names[i], " ", sep="")
    cat(" ", test.functions[[i]](), "\n", sep="")
}
