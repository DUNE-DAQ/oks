package oks;

/**
 * Describe properties of an attribute.
 */

public final class CommitError {

	public enum type_t {

		/** no Access Manager permission */
		no_access_manager_permission("Access Manager grants no permission", "\n"),

		/** Access Manager service error */
		access_manager_failure(null /* TODO: reserved for AM service failure */, null),

		/** database is inconsistent or xml syntax error */
		consistency_error("repository validation failed for file", "\nremote: ERROR: oks validation failed"),

		/** cannot merge database changes */
		merge_conflict("Merge conflict in ", "\n"),
		
		/** cannot merge database changes */
		lock_conflict("failed to lock", "\n"),

		/** git service failure */
		git_failure(null /* TODO: reserved for git service failure */, null),

		/** unknown */
		unknown(null, null);

		private final String m_begin_text;
		private final String m_end_text;

		private type_t(String begin_text, String end_text) {
			m_begin_text = begin_text;
			m_end_text = end_text;
	    }
	};

	private String p_commit_log;
	private String p_error;
	private type_t p_type;

	public CommitError(String log) {
		p_commit_log = log;
        p_error = "";
		p_type = type_t.unknown;
		
		for (type_t e : type_t.values()) {
			String s = search(e.m_begin_text, e.m_end_text);

			if (s.isEmpty() == false) {
				p_error = s;
				p_type = e;
			}
		}

	}

	/** Get the commit log */
	public String get_commit_log() {
		return p_commit_log;
	}
	
	/** Get the error text */
	public String get_error_text() {
		return p_error;
	}
	
	/** Get the error type */
	public type_t get_error_type() {
		return p_type;
	}
	
	private String search(String start_text, String end_text) {
		if(start_text == null)
			return "";

		int begin_idx = p_commit_log.indexOf(start_text);

		if (begin_idx == -1)
			return "";

		int end_idx = p_commit_log.indexOf(end_text, begin_idx);
		
		if (end_idx == -1) {
			end_idx = p_commit_log.indexOf('\n', begin_idx);
		}
		
		return (end_idx == -1) ? p_commit_log.substring(begin_idx) : p_commit_log.substring(begin_idx, end_idx);  
	}
}
