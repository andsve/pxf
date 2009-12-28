(define (apropos name)
  ;; (apropos "name") prints out a list of all symbols whose name includes "name" as a substring

  (define (substring? subs s) ; from Larceny
	(let* ((start 0)
	   (ls (string-length s))
	   (lu (string-length subs))
	   (limit (- ls lu)))
	  (let loop ((i start))
	(cond ((> i limit) #f)
		  ((do ((j i (+ j 1))
			(k 0 (+ k 1)))
		   ((or (= k lu)
			(not (char=? (string-ref subs k) (string-ref s j))))
			(= k lu))) i)
		  (else (loop (+ i 1)))))))

  (define (apropos-1 alist)
	(for-each
	 (lambda (binding)
	   (if (substring? name (symbol->string (car binding)))
	   (format (current-output-port) "~A: ~A~%" 
		   (car binding) 
		   (if (procedure? (cdr binding))
			   (procedure-documentation (cdr binding))
			   (cdr binding)))))
	 alist))

  (for-each
   (lambda (frame)
	 (if (vector? frame) ; the global environment
	 (let ((len (vector-length frame)))
	   (do ((i 0 (+ i 1)))
		   ((= i len))
		 (apropos-1 (vector-ref frame i))))
	 (apropos-1 frame)))
   (current-environment)))
