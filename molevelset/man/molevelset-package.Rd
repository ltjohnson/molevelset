\name{molevelset-package}
\alias{molevelset-package}
\docType{package}
\title{
  Mini-max Optimal Levelsets
}
\description{
  Estimates mini-max optimal levelsets.
}
\details{
\tabular{ll}{
Package: \tab molevelset\cr
Type: \tab Package\cr
Version: \tab 1.0\cr
Date: \tab 2013-09-14\cr
License: \tab GPL-2\cr
LazyLoad: \tab yes\cr
}
}
\author{
Leif Johnson <leif.t.johnson@gmail.com>
}
\references{
  Willet and Nowak (2007) "Minimax Optimal Level Set Estimation."
  \emph{IEEE Transactions on Image Processing}, \bold{16}, 2965--2979.
}
\keyword{ package }
\examples{
X <- matrix(runif(100), ncol=2)
Y <- 0.5 * ((X[, 1] - 0.5)^2 + (X[, 2])^2 < 0.25^2)
Y <- Y + (runif(length(Y)) - 0.5)
levelset.estimate <- molevelset(X, Y, 0.4, 4)
\dontrun{plot(levelset.estimate)}
}
