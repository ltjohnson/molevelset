get.levelset.polygons <- function(le.boxes) {
  # Get polygons representing a levelset.
  #
  # Args:
  #   le.boxes: list of matricies of leveset boxes.
  # Returns:
  #   List of matricies containing polygons verticies
  boxes <- lapply(le.boxes, "[[", "box")
  polygons <- lapply(boxes,
                     function(box) cbind(box[c(1, 1, 2, 2), 1],
                                         box[c(1, 2, 2, 1), 2]))
  return(polygons)                                       
}

get.levelset.lines <- function(le.boxes, k.max) {
  step <- 2**-k.max
  x.edges <- matrix(0, nrow=2**k.max, ncol=2**k.max + 1)
  y.edges <- matrix(0, nrow=2**k.max + 1, ncol=2**k.max)
  boxes <- lapply(le.boxes, "[[", "box")
  for (i in seq_along(boxes)) {
    # Increment the edge count for edge edge in this box.
    box <- boxes[[i]] / step + 1
    x.seq <- seq(box[1, 1], box[2, 1] - 1) 
    y.seq <- seq(box[1, 2], box[2, 2] - 1)
    x.edges[x.seq, box[, 2]] <- x.edges[x.seq, box[, 2]] + 1
    y.edges[box[, 1], y.seq] <- y.edges[box[, 1], y.seq] + 1
  }

  # Edge locations with count == 1 should be plotted because they are on
  # exactly one box border.
  x.edges <- which(x.edges == 1, arr.ind=TRUE)
  y.edges <- which(y.edges == 1, arr.ind=TRUE)

  # Extract the lines.
  f.main <- function(e) rbind(e - 1, e) * step
  f.secondary <- function(e) (rbind(e, e) - 1) * step
  
  x.x <- f.main(x.edges[, 1])
  x.y <- f.secondary(x.edges[, 2])
  x.lines <- lapply(seq_len(ncol(x.x)), function(i) cbind(x.x[, i], x.y[, i]))
  
  y.x <- f.secondary(y.edges[, 1])
  y.y <- f.main(y.edges[, 2])
  y.lines <- lapply(seq_len(ncol(y.y)), function(i) cbind(y.x[, i], y.y[, i]))

  return(c(x.lines, y.lines))
}

.is.color.specification <- function(col) {
  return(!is.null(col) && !is.na(col) && is.character(col) && col !="")
}

.plot.polygons <- function(polys, border=NA, col=NA) {
  if (!.is.color.specification(col)) {
    col <- NA
  }
  if (!.is.color.specification(border)) {
    border <- NA
  }
  for (i in seq_along(polys)) {
    polygon(polys[[i]], border=border, col=col)
  }
}

plot.molevelset <- function(x,
                            point.type=c("both", "inset", "noninset",
                                "neither"),
                            col.inset="gray",
                            col.noninset="gray55",
                            border.inset="red",
                            border.noninset="black",
                            combine.boxes=TRUE,
                            points=TRUE,
                            ...) {
  # Plot a molevelset estimate.
  #
  # Args:
  #   x: object as returned by moleveset.
  #   point.type: character, which kinds of points to plot.
  #   col.inset: color to use for shading inset boxes.  If "", NA, or NULL
  #     then no shading is done for inset boxes.
  #   col.noninset: color to use for shading non-inset boxes.  If "", NA,
  #     or NULL then no shading is done for inset boxes.
  #   border.inset: color to use for borders of inset boxes.  If "", NA, or
  #     NULL then no border is drawn for inset boxes.
  #   border.noninset: color to use for borders of non-inset boxes.  If "",
  #     NA, or NULL then no border is drawn for non-inset boxes.
  #   points: logical, should data points be plotted over the levelset.
  #   ...: additional args passed to plot.
  # Returns:
  #   Nothing.
  point.type <- match.arg(point.type)
  
  # Can we let the user specifiy xlab etc if they want to without messing up
  # our empty default?  Possibly by looking for xlab in ... or adding xlab=""
  # to ... if xlab is not allrady in there.
  plot(c(0, 1), c(0, 1), type="n", xlab="", ylab="", ...)
  
  plot.inset <- point.type %in% c("both", "inset")
  plot.noninset <- point.type %in% c("both", "noninset")

  if (!combine.boxes) {
    if (plot.inset)
      .plot.polygons(get.levelset.polygons(x$inset_boxes),
                     border=border.inset,
                     col=col.inset)
    if (plot.noninset) 
      .plot.polygons(get.levelset.polygons(x$non_inset_boxes),
                     border=border.noninset,
                     col=col.noninset)
  } else {
    if (plot.inset && .is.color.specification(col.inset)) 
      .plot.polygons(get.levelset.polygons(x$inset_boxes),
                     border=NA,
                     col=col.inset)
    if (plot.noninset && .is.color.specification(col.noninset))
      .plot.polygons(get.levelset.polygons(x$non_inset_boxes),
                     border=NA,
                     col=col.noninset)
    if (plot.inset && .is.color.specification(border.inset)) {
      inset.lines <- get.levelset.lines(x$inset_boxes, x$k.max)
      for (i in seq_along(inset.lines))
        lines(inset.lines[[i]], col=border.inset)
    }
    if (plot.noninset && .is.color.specification(border.noninset)) {
      non.inset.lines <- get.levelset.lines(x$non_inset_boxes, x$k.max)
      for (i in seq_along(non.inset.lines))
        lines(non.inset.lines[[i]], col=border.noninset)
    }
  }
  if (points) {
      graphics::points(x$X, col=ifelse(x$Y >= x$gamma, "red", "black"))
  }
}
