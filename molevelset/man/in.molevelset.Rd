\name{in.molevelset}
\alias{in.molevelset}
\title{Compare points to an estimated levelset}
\description{
  See if input points are in a levelset estimate.
}
\usage{
in.molevelset(levelset.estimate, X)
}
\arguments{
  \item{levelset.estimate}{\code{\link{molevelset}} object, the levelset
    estimate.}
  \item{X}{\code{matrix} or \code{data.frame} of coordinates to check.}
}
\details{
  Each row of \code{X} is compared against the levelset estimate
  contained in \code{levelset.estimate}.  \code{X} should be the same
  type as the data used to calculate the levelset estimate.
}
\value{Logical vector with length NROW(X).  Elements are TRUE if the
  corresponding row of X is inside the levelset, FALSE otherwise.
}
\author{
  Leif Johnson <leif.t.johnson@gmail.com>.
}
\keyword{levelset}
\keyword{misc}
\keyword{trees}

