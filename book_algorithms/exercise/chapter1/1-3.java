import java.io.IOException;
import java.util.*;

public class Main {

	public static void main(String[] args) throws IOException {
		// TODO Auto-generated method stub
		int a, b, c;
		Scanner input = new Scanner(System.in);
		a = input.nextInt();
		b = input.nextInt();
		c = input.nextInt();
			
		if (a ==b && b == c)
		{
			System.out.println("equal");
		}
		else
		{
			System.out.println("not equal");
		}
	}

}
