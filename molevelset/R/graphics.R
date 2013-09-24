get.levelset.boxes <- function(levelset.estimate, inset=TRUE) {
    # Get the boxes from a levelset estimate.
    #
    # Args:
    #   le: molevelset object
    #   inset: logical, indicates to get the inset or non-inset boxes.
    # Returns:
    #   List of levelset boxes, the inset boxes if inset is TRUE, the non
    #   inset boxes otherwise.
    boxes <- levelset.estimate$inset_boxes
    if (!inset) {
        boxes <- levelset.estimate$non_inset_boxes
    }
    return(lapply(boxes, "[[", "box"))
}

get.levelset.lines <- function(levelset.estimate, inset=TRUE) {
    # Get the line endpoints for the borer of a levelset estimate.
    #
    # Args:
    #   le: molevelset object.
    #   inset: logical, indicates to get the inset, or non-inset lines.
    boxes <- get.levelset.boxes(le, inset)
    if (!length(boxes)) {
        return(list())
    }
    
    stopifnot(ncol(boxes[[1]]) == 2)
    return(.get.levelset.lines.2d(boxes))
}


get.box.polygons <- function(boxes) {
  # Get polygons representing a box collection.
  #
  # Args:
  #   boxes: list of matricies of boxes.
  # Returns:
  #   List of matricies containing polygons verticies
  boxes <- lapply(boxes, "[[", "box")
  polygons <- lapply(boxes,
                     function(box) cbind(box[c(1, 1, 2, 2), 1],
                                         box[c(1, 2, 2, 1), 2]))
  return(polygons)                                       
}

.expand.lines <- function(lines) {
    # Args:
    #   lines: numeric n x 3 matrix, col1 = line endpoint 1, col2 = line
    #     endpoint 2, col3 = other dimension value.
    # Returns:
    #   m x 3 matrix, having the same column names as lines.  Each row in
    #   lines will correspond to 1 or more rows in the return matrix.  The
    #   corresondence is to expand the line into as many segments as
    #   necessary so that the endpoints do not skip any gridpoints.  Thus,
    #   each row in line will be expanded to k rows, where k is 1 + number
    #   of gridpoints in the interior of the line segment.  Each new row
    #   will have the original col3 value.

    grid.values <- sort(unique(c(lines[, 1], lines[, 2])))

    mat <- cbind(pmin(lines[, 1], lines[, 2]),
                 pmax(lines[, 1], lines[, 2]))
    grid.index <- apply(mat, 1, function(r) c(which(grid.values == r[1]),
                                              which(grid.values == r[2])))
    grid.index <- t(grid.index)
    grid.index <- lapply(seq_len(nrow(grid.index)),
                         function(i) {
                             values <- seq(grid.index[i, 1], grid.index[i, 2])
                             n.values <- length(values)
                             grid.mat <- matrix(c(values[1], rep(values[-c(1, n.values)], each=2), values[n.values]),
                                                ncol=2,
                                                byrow=TRUE)
                             return(cbind(grid.mat, lines[i, 3]))
                         })
    grid.index <- do.call(rbind, grid.index)
                                                    
    full.grid <- cbind(grid.values[grid.index[, 1]],
                       grid.values[grid.index[, 2]],
                       grid.index[, 3])
    colnames(full.grid) <- colnames(lines)
    rownames(full.grid) <- NULL
    return(full.grid)
}

.get.unique.lines <- function(lines.mat) {
    # Args:
    #   lines.mat: n x 3 numeric matrix.
    # Returns:
    #   lines.mat, but containing only the rows that appear exactly
    #   once.
    lines.mat <- lines.mat[order(lines.mat[, 1], lines.mat[, 2],
                                 lines.mat[, 3]), ]
    lines.dup <- duplicated(lines.mat)
    lines.dup.i <- which(lines.dup)
    if (!length(lines.dup.i)) {
        return(lines.mat)
    }
    lines.dup.i <- c(lines.dup.i, lines.dup.i - 1)
    return(lines.mat[-lines.dup.i, , drop=FALSE])
}

.get.levelset.lines.2d <- function(boxes) {
    if (!length(boxes)) {
        return(list())
    }
    stopifnot(ncol(boxes[[1]]) == 2)
    n.dim <- 2

    # Get possible values for all (both) dimensions.
    x.lines <- lapply(boxes,
                      function(b) cbind(X1=b[1, 1], X2=b[2, 1], Y=b[, 2]))
    x.lines <- do.call(rbind, x.lines)
    x.lines <- .expand.lines(x.lines)
    x.lines <- .get.unique.lines(x.lines)
    x.lines <- lapply(seq_len(nrow(x.lines)),
                      function(i) unname(cbind(x.lines[i, 1:2], x.lines[i, 3])))
        
    y.lines <- lapply(boxes,
                      function(b) cbind(Y1=b[1, 2], Y2=b[2, 2], X=b[, 1]))
    y.lines <- do.call(rbind, y.lines)
    y.lines <- .expand.lines(y.lines)
    y.lines <- .get.unique.lines(y.lines)
    y.lines <- lapply(seq_len(nrow(y.lines)),
                      function(i) unname(cbind(y.lines[i, 3], y.lines[i, 1:2])))

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

  # Find our limits by finding the max and min of the boxes in each dimension.
  x.lim <- range(c(unlist(lapply(le$inset_boxes, function(b) b$box[, 1])),
                   unlist(lapply(le$non_inset_boxes, function(b) b$box[, 1]))))
  y.lim <- range(c(unlist(lapply(le$inset_boxes, function(b) b$box[, 2])),
                   unlist(lapply(le$non_inset_boxes, function(b) b$box[, 2]))))
  # Can we let the ouser specifiy xlab etc if they want to without messing up
  # our empty default?  Possibly by looking for xlab in ... or adding xlab=""
  # to ... if xlab is not already in there.
  
  plot(x.lim, y.lim, type="n", xlab="", ylab="", ...)
  
  plot.inset <- point.type %in% c("both", "inset")
  plot.noninset <- point.type %in% c("both", "noninset")

  if (!combine.boxes) {
    if (plot.inset)
      .plot.polygons(get.box.polygons(x$inset_boxes),
                     border=border.inset,
                     col=col.inset)
    if (plot.noninset) 
      .plot.polygons(get.box.polygons(x$non_inset_boxes),
                     border=border.noninset,
                     col=col.noninset)
  } else {
    if (plot.inset && .is.color.specification(col.inset)) 
      .plot.polygons(get.box.polygons(x$inset_boxes),
                     border=NA,
                     col=col.inset)
    if (plot.noninset && .is.color.specification(col.noninset))
      .plot.polygons(get.box.polygons(x$non_inset_boxes),
                     border=NA,
                     col=col.noninset)
    if (plot.inset && .is.color.specification(border.inset)) {
      inset.lines <- get.levelset.lines(lapply(x$inset_boxes, "[[", "box"))
      for (i in seq_along(inset.lines))
        lines(inset.lines[[i]], col=border.inset)
    }
    if (plot.noninset && .is.color.specification(border.noninset)) {
      non.inset.lines <- get.levelset.lines(lapply(x$non_inset_boxes, "[[",
                                                   "box"))
      for (i in seq_along(non.inset.lines))
        lines(non.inset.lines[[i]], col=border.noninset)
    }
  }
  if (points) {
      graphics::points(x$X, col=ifelse(x$Y >= x$gamma, "red", "black"))
  }
}
