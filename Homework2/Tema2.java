import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.nio.file.*;
import java.io.File;
import java.io.FileWriter;

public class Tema2 {
    static String orders;
    static String orderProducts;
    static File ordersFile;
    public static ExecutorService pool;
    
    static Integer P;
    static Integer N = 0;
    static Integer count;
    
    static File productsFile;
    static Path productsPath;
    static long productsSize;

    static String ordersOut;
    static String productsOut;
    static FileWriter writerOrders;
    public static FileWriter writerProducts;

    static Integer ordersLock = 0;
    static Integer productsLock = 0;

    // Each thread will have an ArrayList of indexes
    // The first element will be the start byte where the thread will start reading
    // The second element will be the end byte where the thread will stop reading 
    static ArrayList<ArrayList<Integer>> bytesIndexes = new ArrayList<ArrayList<Integer>>();

    public static void main(String[] args) throws IOException {
        if (args.length < 2) {
            System.out.println("Not enough parameters");
        }

        // Extract input data
        orders = args[0] + "/orders.txt";
        orderProducts = args[0] + "/order_products.txt";
        
        P = Integer.valueOf(args[1]);
        ordersFile = new File(orders);


        // Create the output directory
        ordersOut = "orders_out.txt";
        productsOut = "order_products_out.txt";
        writerOrders = new FileWriter(ordersOut);
        writerProducts = new FileWriter(productsOut);
        

        // Find total size of the file in Bytes and divide it by P
        Double ordersBytes =  new File(orders).length() * 1.0;
        Double chunk = Math.ceil(ordersBytes / P.doubleValue());
        Integer chunkBytes = chunk.intValue();

        // Initialize the thread pool with P workers
        pool = Executors.newFixedThreadPool(P);

        // Each thread will read a chunk of bytes from the file
        Integer startIndex = 0;
        Integer endIndex = ordersBytes.intValue() - 1;
        for (int i = 0; i < P; i++) {
            ArrayList<Integer> indexes = new ArrayList<Integer>();

            // the start byte where thread i will start reading
            indexes.add(startIndex);
            startIndex += chunkBytes.intValue() - 1;

            if (startIndex >= endIndex) {
                startIndex = endIndex;
            }
            
            // the end byte where thread i will stop reading
            indexes.add(startIndex);
            startIndex += 1;

            // start and end bytes for thread i
            bytesIndexes.add(indexes);
        }


        // Initialize 2 * P threads, P for the thread poll
        Thread[] threads = new Thread[P];
        for (int i = 0; i < P; i++) {
        
            Integer start = bytesIndexes.get(i).get(0);
            Integer end = bytesIndexes.get(i).get(1);
            threads[i] = new MyThread(start, end, i, orders, pool, writerOrders, 
                                        writerProducts, orderProducts);
            threads[i].start();
        }
        for (int i = 0; i < P; i++) {
            try {
                threads[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        // Close the thread pool
        pool.shutdownNow();

        // Close the writer
        writerOrders.close();
        writerProducts.close();
    }
}
