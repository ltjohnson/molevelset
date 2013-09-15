\name{molevelset}
\alias{molevelset}
\alias{molevelset.matrix}
\alias{molevelset.formula}
\title{Minimax optimal level set estimation.}
\description{
does some stuff.
}
\usage{
molevelset(X, Y, gamma, k.max, delta=0.05, rho=0.05)
}
\arguments{
  \item{X}{matrix of X coordinates or formula.}
  \item{Y}{Observed function values.  If NULL, and X is a matrix,
    the last column of X is used as the observed function values.}
  \item{gamma}{The threshold for the levelset.}
  \item{k.max}{Maximum number of splits.}
  \item{delta}{PROBABILITY.}
  \item{rho}{Tree complexity penalty multiplier.}
}
\details{
It does stuff.
}
\value{A molevelset object.}
\references{
  Willet and Nowak (2007) "Minimax Optimal Level Set Estimation."
  \emph{IEEE Transactions on Image Processing}, \bold{16}, 2965--2979.
}
\author{
Leif Johnson <leif.t.johnson@gmail.com>.
}
\keyword{ levelset }
\keyword{ trees }
