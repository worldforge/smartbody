(require "asdf")
(load "load.lisp")
;; change revolver to your computer name
(vhmsg:connect :host "revolver")
;; send message example. Note you can't do this here, as there is a bug where the connection is established until after a second or two
;; (vhmsg:post "hello" "hi")
(vhmsg:attach-listener "test" #'vhmsg:sample-listener :id "test-id")
;; this will disconnect (vhmsg:stop)