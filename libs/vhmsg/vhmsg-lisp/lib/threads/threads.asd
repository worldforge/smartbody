;;; -*- Mode: Lisp; Syntax: ANSI-Common-Lisp; Base: 10 -*-
(defsystem threads
  :components (
	       (:file "portable-threads")
	       (:file "scheduled-periodic-functions")
	       )
  :serial t
  )
