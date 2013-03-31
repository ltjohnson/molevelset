get.levelset.polygons <- function(le.boxes, k.max, combine.edges=FALSE) {
  # Get polygons representing a levelset.
  #
  # Args:
  #   le.boxes: list of matricies of leveset boxes.
  #   k.max: numeric, maximum number of splits.
  #   combine.edges: logical, should the boxes be simplified.
  # Returns:
  #   List of matricies containing polygons verticies
  if (combine.edges) {
    waring("Combining edges is not implemented.")
  }
  boxes <- lapply(le.boxes, "[[", "box")
  polygons <- lapply(boxes,
                     function(box) cbind(box[c(1, 1, 2, 2), 1],
                                         box[c(1, 2, 2, 1), 2]))
  return(polygons)                                       
}

.is.color.specification <- function(col) {
  return(!is.null(col) && !is.na(col) && is.character(col) && col !="")
}

.plot.polygons <- function(polys, border=NULL, col=NA) {
  if (!.is.color.specification(col)) {
    col <- NA
  }
  if (!.is.color.specification(border)) {
    border <- NULL
  }
  for (i in seq_along(polys)) {
    polygon(polys[[i]], border=border, col=col)
  }
}

plot.molevelset <- function(le,
                            col.inset="gray",
                            col.noninset="gray55",
                            border.inset="red",
                            border.noninset="black",
                            combine.edges=FALSE,
                            points=TRUE,
                            ...) {
  # Plot a molevelset estimate.
  #
  # Args:
  #   le: object as returned by moleveset.
  #   col.inset: color to use for shading inset boxes.  If "", NA, or NULL
  #     then no shading is done for inset boxes.
  #   col.noninset: color to use for shading non-inset boxes.  If "", NA, or
  #     NULL then no shading is done for inset boxes.
  #   border.inset: color to use for borders of inset boxes.  If "", NA, or
  #     NULL then no border is drawn for inset boxes.
  #   border.noninset: color to use for borders of non-inset boxes.  If "", NA, or
  #     NULL then no border is drawn for non-inset boxes.
  #   points: logical, should data points be plotted over the levelset.
  #   ...: additional args passed to plot.
  # Returns:
  #   Nothing.

  # Can we let the user specifiy xlab etc if they want to without messing up
  # our empty default?  Possibly by looking for xlab in ... or adding xlab=""
  # to ... if xlab is not allrady in there.
  plot(c(0, 1), c(0, 1), type="n", xlab="", ylab="", ...)

  .plot.polygons(get.levelset.polygons(le$inset_boxes,
                                       combine.edges=combine.edges),
                 border=border.inset,
                 col=col.inset)
  .plot.polygons(get.levelset.polygons(le$non_inset_boxes,
                                       combine.edges=combine.edges),
                 border=border.noninset,
                 col=col.noninset)
  if (points)
    graphics::points(le$X, col=ifelse(le$Y >= le$gamma, "red", "black"))
}
