public class Main {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		int M = 5, N = 3;
		int[][] data = { {1, 2, 3}, {4, 5 ,6}, {7 ,8 ,9}, {10, 11, 12},
					{13, 14, 15}};
		int[][] data_rev = new int[N][M];
		for (int i = 0; i < N; ++i)
		{
			for (int j = 0; j < M; ++j)
			{
				data_rev[i][j] = data[j][i];
			}
		
		}
		for (int i = 0; i < N; ++i)
		{
			for (int j = 0; j < M; ++j)
			{
				System.out.print(data_rev[i][j] + " ");;
			}
			System.out.println();
		}
	}

}