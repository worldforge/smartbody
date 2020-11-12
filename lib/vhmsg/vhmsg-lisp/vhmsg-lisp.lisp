(defpackage :vhmsg

  (:use :cl
        :cl-user
	:portable-threads)

  (:export :connect
           :subscribe
	   :stop
	   :post
	   :attach-listener
	   :sample-listener))

(in-package :vhmsg)

(defvar *connection* nil)
(defconstant *default-topic* "/topic/DEFAULT_SCOPE")
(defun connect (&key (host "localhost") (port 61613))
  (setq *connection* (stomp:make-connection host port))
  (spawn-thread "activemq-vhmsg-lisp-connection" #'(lambda ()
						     (stomp:start *connection*))))

(defun attach-listener (prefix listener &key  (topic *default-topic*) id)
  (declare (type string prefix))
  (declare (type function listener))
  (declare (type string id topic))
  (stomp:register *connection* listener topic
		  :selector (if prefix
				(concatenate 'string "MESSAGE_PREFIX = '"
					     prefix
					     "'"))
		  :id id))
(defun detach-listener (id)
  (declare (type string id))
  (stomp:unregister-id *connection* id))

(defun stop ()
  (stomp:stop *connection*)
  (setq *connection* nil))

(defun sample-listener (frame)
  (let ((body (stomp:frame-body frame))
        (headers (stomp:frame-headers frame))
        (name (stomp:frame-name frame)))
    (format t "--> chirp: ~a~%~a~%~a~%" name headers body)))
  
(defun post (prefix rest &optional  (topic *default-topic*))
  (if *connection*
      (stomp:post *connection* (concatenate 'string prefix " " rest)
		  topic '(("ELVISH_SCOPE" "DEFAULT_SCOPE")
			  ("MESSAGE_PREFIX" prefix)))))

