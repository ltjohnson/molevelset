summary.molevelset <- function(object, ...) {
    stopifnot(class(object) == "molevelset")
    summary.list <- list(gamma=object$gamma,
                         k.max=object$k.max,
                         rho=object$rho,
                         delta=object$delta,
                         total.cost=object$total_cost,
                         num.boxes=object$num_boxes,
                         num.inset.boxes=length(object$inset_boxes),
                         num.non.inset.boxes=length(object$non_inset_boxes),
                         num.points=NROW(object$X))
    column.width <- 20
    summary.string <- paste0(
        "molevelset estimate base on ", summary.list$num.points,
        " input points.\n",
        "  Parameters:\n",
        "    ", format("gamma", justify="left", width=column.width), ":",
          format(summary.list$gamma, justify="right", width=column.width,
                 digits=4), "\n",
        "    ", format("k.max", justify="left", width=column.width), ":",
          format(summary.list$k.max, justify="right", width=column.width,
                 digits=4), "\n",
        "    ", format("rho", justify="left", width=column.width), ":",
          format(summary.list$rho, justify="right", width=column.width,
                 digits=4), "\n",
        "    ", format("delta", justify="left", width=column.width), ":",
          format(summary.list$delta, justify="right", width=column.width,
                 digits=4), "\n",
        "  Estimates:\n",
        "    ", format("total.cost", justify="left", width=column.width), ":",
          format(summary.list$total.cost, justify="right", width=column.width,
                 digits=4), "\n",
        "    ", format("num.boxes", justify="left", width=column.width), ":",
          format(summary.list$num.boxes, justify="right", width=column.width,
                 digits=4), "\n",
        "    ", format("num.inset.boxes", justify="left", width=column.width),
          ":",
          format(summary.list$num.inset.boxes, justify="right", width=column.width,
                 digits=4), "\n",
        "    ", format("num.non.inset.boxes", justify="left", width=column.width),
          ":",
          format(summary.list$num.non.inset.boxes, justify="right", width=column.width,
                 digits=4)
        )
    cat(summary.string, "\n", sep="")
    return(invisible(summary.list))
}

print.molevelset <- function(x, ...) {
    stopifnot(class(x) == "molevelset")
    print.string <-
        paste0("molevelset estimate based on ",
               NROW(x$X),
               " points.\n  Total Cost: ",
               round(x$total_cost, 3),
               ".\n  ",
               length(x$inset_boxes),
               " / ",
               x$num_boxes,
               " final boxes in the set.")
    cat(print.string, "\n", sep="")
    return(invisible(print.string))
}
