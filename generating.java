import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.*;

class generating {
	private static void generateMatrix(char type, long N, long M, String nameFile) throws IOException {
		FileOutputStream file = new FileOutputStream(nameFile, false);

		file.write((byte) type);
		ByteBuffer buffer = ByteBuffer.allocate(Long.BYTES);
		buffer.putLong(N);
		for (int i = buffer.array().length - 1; i >= 0; i--)
			file.write(buffer.array()[i]);
		buffer = ByteBuffer.allocate(Long.BYTES);
		buffer.putLong(M);
		for (int i = buffer.array().length - 1; i >= 0; i--)
			file.write(buffer.array()[i]);
		for (int i = 0; i < N; i++)
			for (int j = 0; j < M; j++) {
				Random random = new Random();
				buffer = ByteBuffer.allocate(Float.BYTES);
				Float num= random.nextFloat();
				buffer.putFloat(num);
				for (int k = buffer.array().length - 1; k >= 0; k--)
					file.write(buffer.array()[k]);
			}
		file.close();
	}

	private static void generateVector(char type, long M, String nameFile) throws IOException {
		FileOutputStream file = new FileOutputStream(nameFile, false);

		file.write((byte) type);
		ByteBuffer buffer = ByteBuffer.allocate(Long.BYTES);
		buffer.putLong(M);
		for (int i = buffer.array().length - 1; i >= 0; i--)
			file.write(buffer.array()[i]);
		for (int j = 0; j < M; j++) {
			Random random = new Random();
			buffer = ByteBuffer.allocate(Float.BYTES);
			Float num= random.nextFloat();
			buffer.putFloat(num);
			for (int k = buffer.array().length - 1; k >= 0; k--)
				file.write(buffer.array()[k]);
		}
		file.close();
	}

	public static void main(String[] args) throws IOException {
		char type = args[0].charAt(0);
		long N = Long.parseLong(args[1]);
		long M = Long.parseLong(args[2]);
		String fileA = args[3];
		String fileB = args[4];
		generateMatrix(type, N, M, fileA);
		generateVector(type, M, fileB);
	}
}