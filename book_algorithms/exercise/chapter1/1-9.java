public class Main {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		int N = 127;
		String strN = "";
		while (N > 0)
		{
			strN = strN + N % 2;
			N = N / 2;
		}
		System.out.println(strN);
	}

}
