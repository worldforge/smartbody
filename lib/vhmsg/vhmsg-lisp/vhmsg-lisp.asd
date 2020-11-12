(in-package :asdf)

(defsystem :vhmsg-lisp
  :description "VH message library for LISP"
  :author "Fabrizio Morbini"
  :depends-on (:cl-stomp :threads)
  :components ((:file "vhmsg-lisp")))
