;;; MAKE.M
;;; BRIEF macro for invoking "make" on a buffer.  Styled after the cc macro.
;;;
;;;	D. Kneller
;;;	Oct 20, 1987
;;;
;;; For my copy of BRIEF, I assign the make macro to function key Alt-F9.
;;; (assign_to_key "%#112" "make")	;** Assigned to Alt-F9.

(macro make (
	(string	cmdline file_name)
	(int	loc status)
	(extern	next_error)

	(inq_names file_name)		; Get full pathname of file

	;; If the file has been modified, write it out.
	(if (inq_modified) (
		(int	old_msg_level)

		(= old_msg_level (inq_msg_level))
		(set_msg_level 0)
		(= status (write_buffer))
		(set_msg_level old_msg_level)
		(if (<= status 0) (
			(message "Write before make failed")
			(return)
		))
	))

		
	;; Strip the path off the front and the extension off the back
	;; of the file_name.
	(= loc (rindex file_name (substr file_name 3 1)))
	(if (> loc 0)
		(= file_name (substr file_name (+ loc 1))))
	(= loc (index file_name "."))
	(if (> loc 0)
		(= file_name (substr file_name 1 (- loc 1))))


	;; Prepare and execute the command string.
	(message "make %s.obj" file_name)
	(sprintf cmdline "make -m INBRIEF=1 %s.obj >& %s.err"
		file_name file_name)
	(delete_buffer (create_buffer "Make Errors" (+ file_name ".err") 1))
	(= status (dos cmdline 0))


	;; Use next_error function if compile failed.
	(if (> status 0)
		(next_error)
		((del (+ file_name ".err"))
		 (message "Make successful."))
	)
))
