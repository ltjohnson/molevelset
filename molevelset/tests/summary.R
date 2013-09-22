library(molevelset)

TestPrintString <- function() {
    X <- matrix(0.5, ncol=2, nrow=10)
    Y <- rep(1, NROW(X))
    le <- molevelset(X, Y, gamma=0, k.max=2)
    expected <- paste("molevelset estimate based on 10 points.",
                      "   Total Cost: -2.313.",
                      "   1 / 1 final boxes in the set.",
                      sep="\n")
    stopifnot(expected == print(le))
    return(TRUE)
}

TestSummaryList <- function() {
    X <- matrix(0.5, ncol=2, nrow=10)
    Y <- rep(1, NROW(X))
    le <- molevelset(X, Y, gamma=0, k.max=2)
    actual <- summary(le)

    expected <-
        structure(list(total_cost = -2.31276695516712, gamma = 0, k.max = 2, 
                       rho = 0.05, delta = 0.05,
                       total.cost = -2.31276695516712, 
                       num.boxes = 1, num.inset.boxes = 1L,
                       num.non.inset.boxes = 0L, 
                       num.points = 10L),
                  .Names = c("total_cost", "gamma", "k.max", 
                      "rho", "delta", "total.cost", "num.boxes",
                      "num.inset.boxes", "num.non.inset.boxes", "num.points"))
    stopifnot(identical(expected, actual))
}

test.names <- ls(pattern="^Test.*")
test.functions <- lapply(test.names, get)
test.i <- which(sapply(test.functions, class) == "function")

for (i in test.i) {
    cat(test.names[i], " ", sep="")
    cat(" ", test.functions[[i]](), "\n", sep="")
}

