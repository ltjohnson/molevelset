#################################################################
# support functions for level set calculation
#################################################################
# take a box formatted as a list of splits, and turn it into a hash key
boxtokey <- function(box) {
  tmp <- sapply(box, function(l) paste(l, collapse=''))
  paste('a', paste(tmp, collapse='a'), sep='')
}

keytobox <- function(key) {
  tmp <- strsplit(key, 'a')[[1]][-1]
  lapply(tmp, function(l) as.numeric(strsplit(l, '')[[1]]))
}

# calculate the splits for a given coordinate.  UNIVARIATE.
findlevel <- function(x, kmax=3) {
  res <- rep(0, kmax)
  bmin <- 0
  bmax <- 1
  bmid <- .5
  for(i in 1:kmax) {
    if (x < bmid) {
      res[i] <- 1
      bmax <- bmid
      bmid <- (bmin + bmid) / 2
    } else {
      res[i] <- 2
      bmin <- bmid
      bmid <- (bmax + bmid) / 2
    }
  }
  res
}

# use splits to find the corners of a box.
splittobox <- function(split) {
  x1 <- 0
  x2 <- 1
  for( i in split ) {
    if (i == 1)
      x2 <- (x1 + x2)/2
    else
      x1 <- (x1 + x2)/2
  }
  c(x1, x2)
}

findbox <- function(x, kmax=3) {
  lapply(x, function(i) findlevel(i, kmax))
}

newbox <- function(box=NULL, Y=NULL, id=NULL) {
  b <- list(box=box, id=id, Y=Y, nbox=0, cost=NULL, children=NULL, 
            inset=NULL, checked=NULL)
  if (!is.null(Y)) {
    b$nbox <- 1
  }
  if (!is.null(box))
    b$checked <- rep(FALSE, length(box))
  b
}

# create a hash containing the box assignments for the input points 
# this could be a slow point, but we have bigger fish to fry
getboxes <- function(X, Y=NULL, kmax=3) {
  d <- ncol(X)
  h <- hash()
  if (is.null(Y)) {
    Y <- X[,d]
    X <- X[,-d]
    d <- d-1
  }
  for (i in 1:nrow(X)) {
    x <- X[i,]
    y <- Y[i]
    box <- findbox(x, kmax)
    key <- boxtokey(box)
    b <- newbox(box=box, Y=y, id=i)
    if (has.key(key, h)) {
      b$Y <- c( h[[key]]$Y, b$Y)
      b$id <- c( h[[key]]$id, b$id)
      b$nbox <- h[[key]]$nbox + 1
    }
    h[[ key ]] <- b
  }
  h
}

# find the Y values for a box
getY <- function(box) {
  if (is.null(box$children)) 
    return( box$Y )
  return( c( getY(box$children[[1]]), getY(box$children[[2]]) ) )
}

# find the ids for a box.
getid <- function(box) {
  if (is.null(box$children))
    return( box$id )
  return( c( getid(box$children[[1]]), getid(box$children[[2]]) ) )
}

# compute complexity penalty for a box
phi <- function(box, N, delta) {
  L <- max(sapply(box$box, length)) * (log2(length(box$box)) + 2) + 1
  pnew <- 4 * max(box$nbox, L * log(2) - log(delta) ) / N
  sqrt( (8 * log(2/delta) + L * log(2)) * pnew / N )
}

# compute the risk for a box.
risk <- function(box, gamma, A) {
  r <- 0
  Y <- getY(box)
  if (length(Y))
    r <- sum(gamma - Y) / (2 * A)
  if (r >= 0) 
    return( list(risk=-r, inset=FALSE) )
  return( list(risk=r, inset=TRUE) )
}

# compute the total cost for a box
getcost <- function(box, N, gamma, delta, rho, A) {
  r <- risk(box, gamma, A) 
  list(cost=r$risk + rho * phi(box, N, delta), inset=r$inset)
}


