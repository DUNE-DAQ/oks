package oks;

public class TestCommitError {

	public static void main(String args[]) {
		StringBuilder log = new StringBuilder();
		
		// read log from standard input
		java.util.Scanner in = new java.util.Scanner(System.in);
		while (in.hasNextLine()) {
			log.append(in.nextLine());
			log.append("\n");
		}
		in.close();
        
		CommitError error = new CommitError(log.toString());
		
		System.out.println("type: " + error.get_error_type().name());
		System.out.println("error: \"" + error.get_error_text() + "\"");

	}
	
}