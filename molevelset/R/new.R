box.points <- function(X, k.max) {
  .Call("get_boxes", X, as.integer(k.max))
}

molevelset <- function(X, Y, gamma, k.max=3, delta=0.05, rho=0.05) {
  le <- .Call("estimate_levelset", X, Y, as.integer(k.max), gamma, delta, rho)
  le$X <- X
  le$Y <- Y
  le$k.max <- k.max
  le$gamma <- gamma
  le$delta <- delta
  le$rho <- rho
  return(le)
}
