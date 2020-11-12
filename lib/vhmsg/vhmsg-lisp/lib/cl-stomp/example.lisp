;;; -*- mode: lisp; indent-tabs-mode: nil; -*-

(defpackage :stomp-example
  (:use :cl
        :cl-user))

(in-package :stomp-example)


(defparameter *stomp* nil)
(defparameter *health-request* "/topic/DEFAULT_SCOPE")
(defparameter *health-response* "/topic/health-response")
(defparameter *host* "localhost")     ;or "localhost"
(defparameter *port* 61613)     ;by convention
(defparameter *counter* 0)

(defun out (fmt &rest arguments)
  (apply #'format *standard-output* fmt arguments)
  (finish-output *standard-output*))

(defun callback (frame)
  (incf *counter*)
  (out "[~a]~%" (stomp:frame-body frame))
  (let ((msg (format nil "<pong><name>foo</name><count>~a</count></pong>" *counter*)))
    (stomp:post *stomp* msg *health-response*)))

(defun bark (frame)
  (let ((body (stomp:frame-body frame)))
    (out "--> bark : ~a~%" body)))

(defun chirp (frame)
  (let ((body (stomp:frame-body frame))
        (headers (stomp:frame-headers frame))
        (name (stomp:frame-name frame)))
    (out "--> chirp: ~a~%~a~%~a~%" name headers body)
    (when (equal body "asd ")
      (out "posting")
      (stomp:post *stomp* "test" *health-request* '(("ELVISH_SCOPE" "DEFAULT_SCOPE"))))))

(defun run ()
  (setf *stomp* (stomp:make-connection *host* *port*))
  ;(stomp:register *stomp* #'callback *health-request*)
  (stomp:register *stomp* #'chirp *health-request*)
  ;(stomp:register *stomp* #'bark *health-request*)
  (stomp:start *stomp*))

(defun start ()
  #+sbcl (sb-thread:make-thread (lambda () (run)) :name "stomp-subscribe")
  #+ccl  (ccl:process-run-function "stomp-subscribe" (lambda () (run))))

(defun stop ()
  (stomp:stop *stomp*)
  (setf *stomp* nil))
