library(molevelset)

TestGetBoxPolygons <- function() {
    boxes <- list(list(box=cbind(c(1, 2), c(3, 4))),
                  list(box=cbind(c(1, 2), c(4, 5))))
    expected <- list(cbind(c(1, 1, 2, 2), c(3, 4, 4, 3)),
                     cbind(c(1, 1, 2, 2), c(4, 5, 5, 4)))
    actual <- molevelset:::get.box.polygons(boxes)
    stopifnot(isTRUE(all.equal(expected, actual)))
    return(TRUE)
}

TestExpandLines <- function() {
    lines <- rbind(c(1, 2, 3),
                   c(1, 3, 4),
                   c(2, 4, 3))
    expected <- rbind(c(1, 2, 3),
                      c(1, 2, 4),
                      c(2, 3, 4),
                      c(2, 3, 3),
                      c(3, 4, 3))
    stopifnot(isTRUE(all.equal(expected, molevelset:::.expand.lines(lines))))
    return(TRUE)
}

TestGetUniqueLines <- function() {
    lines <- rbind(c(1, 2, 3),
                   c(1, 3, 4),
                   c(2, 4, 3))
    stopifnot(isTRUE(all.equal(lines, molevelset:::.get.unique.lines(lines))))
    
    lines <- rbind(c(1, 2, 3),
                   c(1, 2, 3),
                   c(1, 2, 4))
    expected <- matrix(c(1, 2, 4), ncol=3)
    stopifnot(isTRUE(all.equal(expected,
                               molevelset:::.get.unique.lines(lines))))

    return(TRUE)
}

TestGetLevelsetLines2d <- function() {
    stopifnot(isTRUE(all.equal(list(),
                               molevelset:::.get.levelset.lines.2d(list()))))
    stopifnot(isTRUE(all.equal(list(),
                               molevelset:::.get.levelset.lines.2d(NULL))))

    boxes <- list(cbind(c(1, 2), c(3, 4)))
    expected <- list(cbind(1:2, 3),
                     cbind(1:2, 4),
                     cbind(1, 3:4),
                     cbind(2, 3:4))
    stopifnot(isTRUE(all.equal(expected,
                               molevelset:::.get.levelset.lines.2d(boxes))))

    boxes <- list(cbind(1:2, 3:4),
                  cbind(1:2, 4:5))
    expected <- list(cbind(as.integer(1:2), as.integer(3)),
                     cbind(as.integer(1:2), as.integer(5)),
                     cbind(as.integer(1), as.integer(3:4)),
                     cbind(as.integer(2), as.integer(3:4)),
                     cbind(as.integer(1), as.integer(4:5)),
                     cbind(as.integer(2), as.integer(4:5)))
    stopifnot(isTRUE(all.equal(expected,
                               molevelset:::.get.levelset.lines.2d(boxes))))

    return(TRUE)
}

test.names <- ls(pattern="^Test.*")
test.functions <- lapply(test.names, get)
test.i <- which(sapply(test.functions, class) == "function")

for (i in test.i) {
    cat(test.names[i], " ", sep="")
    cat(" ", test.functions[[i]](), "\n", sep="")
}
