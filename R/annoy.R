
## ensure module gets loaded
loadModule("AnnoyIF", TRUE)

AnnoyIndex <- function(n, metric="angular") {
  annoy <- switch(metric,
    angular=new(AnnoyIF, n, 0),
    euclidean=new(AnnoyIF, n, 1))

  # wrap for formal arguments (maybe there's a better way?)
  assign("getNNsByItem", function(item, n, search_k=-1, include_distances=FALSE) {
    annoy$`_getNNsByItem`(item, n, search_k, include_distances)
    }, envir=annoy)
  assign("getNNsByVector", function(w, n, search_k=-1, include_distances=FALSE) {
    annoy$`_getNNsByVector`(w, n, search_k, include_distances)
    }, envir=annoy)

  annoy
}
