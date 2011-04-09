\name{molevelset}
\alias{molevelset}
\title{Minimax optimal level set estimation.}
\description{
does some stuff.
}
\usage{
molevelset(X, Y = NULL, gamma, k.max = 3, rho = 0.05)
}
\arguments{
  \item{X}{X coordinates.}
  \item{Y}{Observed function values.  If NULL, the last column of X is used.}
  \item{gamma}{The threshold for the levelset.}
  \item{k.max}{Maximum number of splits.}
  \item{rho}{Tree complexity penalty multiplier.}
}
\details{
It does stuff.
}
\value{ A Matrix of the boxes.  }
\references{
Willet and Nowak.
}
\author{
Leif Johnson.
}
\keyword{ levelset }
\keyword{ trees }
