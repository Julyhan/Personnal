public class Main {

	public static int lg(int N){
		if (N <= 0 )
		{
			System.out.println("Error!");
			return 0;
		}
		int result = 0;
		double n_double = N;
		while (n_double >= 2)
		{
			n_double /= 2;
			result++;
		}
		return result;
	}
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		for (int i = -1; i < 17; ++i)
		{
			System.out.println(i + ": " + lg(i));
		}
	}

}