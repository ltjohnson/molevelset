library(molevelset)

###########################################################################
## Circle in the upper right example.
n <- 200
k.max <- 6
X <- expand.grid(seq(0, 1, length.out=n), seq(0, 1, length.out=n))
X <- as.matrix(X)
Y <- ifelse(rowSums((X - 1)^2) <= (0.2 * sqrt(2))^2, 1, 0)

le <- molevelset(X, Y, k.max=k.max, gamma=0.5, rho=0)
plot.molevelset(le, points=FALSE, combine.boxes=TRUE,
                col.noninset=NA, border.noninset=NA,
                col.inset="black", border.inset=NA)

le1 <- molevelset(X, Y, k.max=k.max, gamma=0.5, rho=1)
plot.molevelset(le1, points=FALSE, combine.boxes=TRUE,
                col.noninset=NA, border.noninset=NA,
                col.inset="black", border.inset=NA)


le100 <- molevelset(X, Y, k.max=k.max, gamma=0.5, rho=100)
plot.molevelset(le100, points=FALSE, combine.boxes=TRUE,
                col.noninset=NA, border.noninset=NA,
                col.inset="black", border.inset=NA)

le1000 <- molevelset(X, Y, k.max=k.max, gamma=0.5, rho=1000)
plot.molevelset(le1000, points=FALSE, combine.boxes=TRUE,
                col.noninset=NA, border.noninset=NA,
                col.inset="black", border.inset=NA)

le5000 <- molevelset(X, Y, k.max=k.max, gamma=0.5, rho=5000)
plot.molevelset(le5000, points=FALSE, combine.boxes=TRUE,
                col.noninset=NA, border.noninset=NA,
                col.inset="black", border.inset=NA)

le7500 <- molevelset(X, Y, k.max=k.max, gamma=0.5, rho=7500)
plot.molevelset(le7500, points=FALSE, combine.boxes=TRUE,
                col.noninset=NA, border.noninset=NA,
                col.inset="black", border.inset=NA)


le10000 <- molevelset(X, Y, k.max=k.max, gamma=0.5, rho=10000)
plot.molevelset(le1000, points=FALSE, combine.boxes=TRUE,
                col.noninset=NA, border.noninset=NA,
                col.inset="black", border.inset=NA)

###########################################################################
## Diagonal split
X <- matrix(runif(200), ncol=2)
Y <- 0.4 + 0.2 * (X[, 1] < X[, 2])
# To add noise
#Y <- Y + rnorm(nrow(X), sd=0.1)
gamma <- 0.5
plot(X, col=ifelse(Y >= gamma, "red", "black"))

le <- molevelset(X, Y, k.max=5, gamma=0.5, rho=0)
plot.molevelset(le, combine.boxes=TRUE)
abline(0, 1, lwd=3, lty=2)

le <- molevelset(X, Y, k.max=5, gamma=0.5, rho=0.05)
plot.molevelset(le)
abline(0, 1, lwd=3, lty=2)

le <- molevelset(X, Y, k.max=5, gamma=0.5, rho=1.00)
plot.molevelset(le)
abline(0, 1, lwd=3, lty=2)

# Same example, but a regular collection of points.
X <- expand.grid(seq(0, 1, by=2**-5), seq(0, 1, by=2**-5))
X <- unname(as.matrix(X))
Y <- 1 * (X[, 1] < X[, 2])

le <- molevelset(X, Y, k.max=3, gamma=0.5, rho=0)
plot.molevelset(le, combine.boxes=TRUE, which="inset")
abline(0, 1, lwd=3, lty=2)

le <- molevelset(X, Y, k.max=4, gamma=0.5, rho=0)
plot.molevelset(le, combine.boxes=TRUE)
abline(0, 1, lwd=3, lty=2)

le <- molevelset(X, Y, k.max=5, gamma=0.5, rho=0.10)
plot.molevelset(le)
abline(0, 1, lwd=3, lty=2)

plot.molevelset(le, combine.boxes=TRUE)