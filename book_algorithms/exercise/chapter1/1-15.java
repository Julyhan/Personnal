public class Main {

	public static int[] histogram(int[] a, int M){
		int[] result = new int[M];
		if (a.length != M)
		{
			System.out.println("M is Error!");
			return result;
		}
		for (int i = 0; i < M; i++){
			result[i] = 0;
		}
		for (int i = 0; i < M; i++){
			if (a[i]>=0 && a[i]<M)
			{
				result[a[i]]++;
			}
		}
		return result;
	}
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		int M = 5;
		int[] input = new int[M];
		for (int i = 0; i < M; ++i){
			input[i] = (int) Math.round(Math.random() * M);
			System.out.print(input[i] + " ");
		}
		System.out.println();
		int[] result = histogram(input, M);
		for (int i = 0; i < input.length; ++i){
			System.out.println(result[i]);
		}
	}

}