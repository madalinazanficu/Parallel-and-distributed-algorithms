import java.io.IOException;
import java.util.stream.*;
import java.nio.file.*;


public class Main {
    static String orders;
    static String orderProducts;
    static Path ordersPath;
    static Integer P;
    static Integer count;

    public static void main(String[] args) throws IOException {
        if (args.length < 2) {
            System.out.println("Not enough parameters");
        }

        // Extract input data
        orders = args[0] + "/orders.txt";
        orderProducts = args[0] + "/order_products.txt";
        P = Integer.valueOf(args[1]);
        
        // Count the number of commands
        ordersPath = Paths.get(orders);
        long ordersSize = Files.lines(ordersPath).count();
        
        // Distribute equally the number of commands to P theards
        Thread[] threads = new Thread[P];
        if (ordersSize > P) {
            Double division = Math.ceil(ordersSize / P.doubleValue());
            count  = division.intValue();
        } else {
            count = 1;
        }

        for (int i = 0; i < P; i++) {
        
            // Start the thread if there are commands to be executed
            if (i * count < ordersSize) {
                long start = i * count;
                long end = start + count - 1;

                if (end >= ordersSize) {
                    end = ordersSize - 1;
                }
            
                threads[i] = new MyThread(start, end, i);
                threads[i].start();
            }
        }
        
        //File ordersFile = new File(orders);
        //File productsFile = new File(orderProducts);

        

        String line;
        try (Stream<String> lines = Files.lines(Paths.get(orders))) {
            line = lines.skip(1).findFirst().get();
            System.out.println(line);
        }
    }
}
