find.sibling <- function(box, k, D) {
  sib <- box$box
  nk <- length(sib[[k]])
  # flip the split in dimension k.
  sib[[k]][nk] <- ifelse(sib[[k]][nk] == 1, 2, 1)
  sib.key <- boxtokey(sib)
  if (has.key(sib.key, D)) {
    # box already exists.
    sib <- D[[ sib.key ]]
  } else {
    # box doesn't exist, make up an empty one.
    sib <- newbox(box=sib)
    sib$inset <- FALSE
    sib$cost <- getcost(sib, N, gamma, delta, rho, A)$cost
  }
  sib
}

find.parent <- function(box, sib, k, D) {
  parent <- box$box
  # delete the split in dimension k.
  nk <- length(parent[[k]])
  parent[[k]] <- parent[[k]][ -nk ]
  parent.key <- boxtokey(parent)
  if (has.key(parent.key, D))
    return( D[[ parent.key ]] )
  # parent doesn't exist, make it up.
  parent <- newbox(box=parent)
  parent$children <- list(box, sib)
  parent$nbox <- box$nbox + sib$nbox
  parent
}

molevelset <- function(X, Y=NULL, gamma=0.5, kmax=3, rho=0.01) {
  # some constants needed below
  delta <- 0.05
  if (is.null(Y)) {
    Y <- X[,ncol(X)]
    X <- X[,-ncol(X)]
  }
  A <- max(abs(Y))
  # number of dimensions and sample size.
  d <- ncol(X)
  N <- nrow(X)

  # initialize list of collections of boxes and deepest level.
  D <- lapply(1:(d*kmax+1), function(i) hash())
  D[[d*kmax+1]] <- getboxes(X, Y, kmax) 
  for(key in keys(D[[d*kmax+1]])) {
    box <- D[[d*kmax+1]][[ key ]] 
    cost <- getcost(box, N, gamma, delta, rho, A)
    box$cost  <- cost$cost
    box$inset <- cost$inset
    D[[d*kmax+1]][[ key ]] <- box
  } 

  for(depth in (d*kmax+1):2) {
    for(key in keys(D[[depth]])) {
      box <- D[[depth]][[key]]
      if (is.null(box$checked)) 
        box$checked <- rep(FALSE, d)
      for (k in 1:d) {

        # don't check missing splits, and don't double check a sibling.
        if (length(box$box[[k]]) == 0 || box$checked[k])
          next

        # find the sibling (in this dimension) and parent boxes.
        sib <- find.sibling(box, k, D[[depth]]) 
        parent <- find.parent(box, sib, k, D[[depth - 1]])
        if (is.null(parent$cost)) {
          # calculate cost as if the parent box were a terminal node
          cost <- getcost(parent, N, gamma, delta, rho, A)
          parent$cost  <- cost$cost
          parent$inset <- cost$inset
        }
        if (sib$cost + box$cost < parent$cost) {
          # it's better to keep this split than what's already there!
          parent['inset'] <- list(NULL)
          parent$cost <- sib$cost + box$cost
          parent$children <- list(box, sib)
        }
        D[[depth-1]][[ boxtokey(parent$box) ]] <- parent

        # mark this sibling pair as checked
        box.key <- boxtokey(box$box)
        sib.key <- boxtokey(sib$box)
        box$checked[k] <- TRUE
        sib$checked[k] <- TRUE
        
        if (has.key(boxtokey(sib$box), D[[depth]])) {
          # don't add the empty box
          D[[depth]][[ sib.key ]] <- sib
        }
        D[[depth]][[ box.key ]] <- box
      }
    }
  }
  # D_0 (D[[1]]) now contains the best tree
  D[[1]][[ keys(D[[1]])[1] ]]
}

# list boxes in a given set
listboxes <- function(box) {
  if (is.null(box$inset))
    return( rbind(listboxes(box$children[[1]]), listboxes(box$children[[2]])) )
  if (!box$inset) 
    return( NULL )
  c(sapply(box$box, splittobox))  
}
