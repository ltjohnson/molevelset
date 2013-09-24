\name{levelset-util}
\alias{get.levelset.boxes}
\alias{get.levelset.lines}
\title{Access functions for levelset estimates.}
\description{
  Access elements of levelsets estimates.
}
\usage{
get.levelset.boxes(levelset.estimate, inset=TRUE)
get.levelset.lines(levelset.estimate, inset=TRUE)
}
\arguments{
  \item{levelset.estimate}{\code{\link{molevelset}} object, the levelset
    estimate.}
  \item{inset}{\code{logical}, indicates to access the elements from the
    inset, or non-inset estimates.}
}
\details{
  See \code{\link{molevelset}} for the definition of a \code{box}.

  If \code{inset} is \code{TRUE}, \code{get.levelset.boxes} returns a
  list of the boxes making up the levelset estimate and
  \code{get.levelset.lines} returns a list of line segments making up
  the border of the levelset estimate.

  
  If \code{inset} is \code{FALSE}, \code{get.levelset.boxes} returns a
  list of the boxes that are estimated to not be in the levelset and
  \code{get.levelset.lines} returns a list of line segments giving the
  border of the non-inset levelset estimate.
}
\value{\code{get.levelset.boxes} and \code{get.levelset.lines} return a
  list of \code{boxes} and a list of line segments, respectively.}
\author{
  Leif Johnson <leif.t.johnson@gmail.com>.
}
\examples{
X <- cbind(c(0.25, 0.25, 0.75, 0.75),
           c(0.25, 0.75, 0.75, 0.25))
Y <- c(0, 1, 1, 0)
le <- molevelset(X, Y, gamma=0.5, k.max=2, rho=0.01)

# inset.boxes will contain the box representing the top half of the
# unit square.  inset.lines will contain the line segments of that
# box.
inset.boxes <- get.levelset.boxes(le)
inset.lines <- get.levelset.lines(le)

# non.inset.boxes will contain the box representing the bottom half
# of the unit square.  non.inset.lines will contain the line segments
# of same.
non.inset.boxes <- get.levelset.boxes(le, inset=FALSE)
non.inset.lines <- get.levelset.lines(le, inset=FALSE)

}
\keyword{levelset}
\keyword{misc}
\keyword{trees}
