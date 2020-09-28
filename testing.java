import java.io.FileInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;

class testing {
	private static double EPS = 1e-20;

	private static byte[] reverseByteMassive(byte [] number) {
		for (int i = 0; i < number.length / 2; i++) {
			byte help = number[i];
			number[i] = number[number.length - i - 1];
			number[number.length - i - 1] = help;
		}
		return number;
	}

	private static long readLong(FileInputStream file) throws IOException {
		byte [] num = new byte[8];
		file.read(num);
		return ByteBuffer.wrap(reverseByteMassive(num)).getLong();
	}

	private static float readFloat(FileInputStream file) throws IOException {
		byte [] num = new byte[4];
		file.read(num);
		return ByteBuffer.wrap(reverseByteMassive(num)).getFloat();
	}

	private static double readDouble(FileInputStream file) throws IOException {
		byte [] num = new byte[8];
		file.read(num);
		return ByteBuffer.wrap(reverseByteMassive(num)).getDouble();
	}

	private static void readMatrix(String fileName, Float [][] fMatrix, Double [][] dMatrix, int size) throws IOException {
		FileInputStream file = new FileInputStream(fileName);
		char type = (char) file.read();

		long N = readLong(file);
		long M = readLong(file);
		for (int i = 0; i < N; i++)
			for (int j = 0; j < M; j++)
				if (size == 4)
					fMatrix[i][j] = readFloat(file);
				else
					dMatrix[i][j] = readDouble(file);
		file.close();
	}

	public static void main(String[] args) throws IOException {
		String fileC = args[0];
		FileInputStream file = new FileInputStream(fileC);
		char type = (char) file.read();
		if (type == 'f') {
			long N = readLong(file);
			long M = readLong(file);
			System.out.print("Testing float matrix " + N + "x" + M + " with " + args[2] + " : ");
			Float [][] matrixC = new Float[(int)N][(int)M];
			for (int i = 0; i < N; i++)
				for (int j = 0; j < M; j++)
					matrixC[i][j] = readFloat(file);
			file.close();
			Float [][] matrixC_test = new Float[(int)N][(int)M];
			readMatrix(args[1], matrixC_test, null, 4);
			for (int i = 0; i < N; i++)
				for (int j = 0; j < M; j++)
					if (Math.abs(matrixC[i][j] -matrixC_test[i][j]) > EPS)
						assert(false);
		} else if (type == 'd') {
			long N = readLong(file);
			long M = readLong(file);
			System.out.print("Testing double matrix " + N + "x" + M + " with " + args[2] + " : ");
			Double [][] matrixC = new Double[(int)N][(int)M];
			for (int i = 0; i < N; i++)
				for (int j = 0; j < M; j++)
					matrixC[i][j] = readDouble(file);
			file.close();
			Double [][] matrixC_test = new Double[(int)N][(int)M];
			readMatrix(args[1], null, matrixC_test, 8);
			for (int i = 0; i < N; i++)
				for (int j = 0; j < M; j++)
					if (Math.abs(matrixC[i][j] -matrixC_test[i][j]) > EPS)
						assert(false);
		}
		System.out.println("OK ✅");
	}
}