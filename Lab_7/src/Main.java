import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

class ThreadArg {
    public List<Integer> array = new ArrayList<>();
    public int K;
    public final Object mutex = new Object();
    public boolean eventOccurred = false;
    public final Object cs = new Object();
    public List<Integer> resultArray = new ArrayList<>();
    public int nonZeroCount;
}

public class Main {
    private static int countOnesInDecimal(int num) {
        int count = 0;
        while (num != 0) {
            if (num % 10 == 1) {
                count++;
            }
            num /= 10;
        }
        return count;
    }

    static class WorkRunnable implements Runnable {
        private final ThreadArg threadArg;

        WorkRunnable(ThreadArg threadArg) {
            this.threadArg = threadArg;
        }

        @Override
        public void run() {
            synchronized (threadArg.mutex) {
                System.out.println("Work thread is starting...");

                for (int num : threadArg.array) {
                    if (countOnesInDecimal(num) == threadArg.K) {
                        threadArg.resultArray.add(num);
                    }
                }

                synchronized (threadArg) {
                    threadArg.eventOccurred = true;
                    threadArg.notifyAll();
                }

                System.out.println("Work thread is ending...");
            }
        }
    }

    static class CountElementRunnable implements Runnable {
        private final ThreadArg threadArg;

        CountElementRunnable(ThreadArg threadArg) {
            this.threadArg = threadArg;
        }

        @Override
        public void run() {
            synchronized (threadArg.cs) {
                // Ждем завершения Work
                synchronized (threadArg) {
                    while (!threadArg.eventOccurred) {
                        try {
                            threadArg.wait();
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                    }
                }

                System.out.println("CountElement thread is starting...");

                threadArg.nonZeroCount = 0;
                for (int num : threadArg.array) {
                    if (num != 0) {
                        threadArg.nonZeroCount++;
                    }
                }

                System.out.println("CountElement thread is ending...");
            }
        }
    }

    public static void main(String[] args) {
        ThreadArg threadArg = new ThreadArg();
        Scanner scanner = new Scanner(System.in);

        System.out.print("Enter array size: ");
        int size = scanner.nextInt();

        System.out.println("Enter array elements:");
        for (int i = 0; i < size; i++) {
            System.out.print("Element " + (i + 1) + ": ");
            threadArg.array.add(scanner.nextInt());
        }

        System.out.print("Array: ");
        for (int num : threadArg.array) {
            System.out.print(num + " ");
        }
        System.out.println();

        // Захватываем мьютекс перед запуском потоков
        synchronized (threadArg.mutex) {
            new Thread(new WorkRunnable(threadArg)).start();
            new Thread(new CountElementRunnable(threadArg)).start();

            System.out.print("Enter K: ");
            threadArg.K = scanner.nextInt();
        }

        // Ожидаем завершения Work
        synchronized (threadArg) {
            while (!threadArg.eventOccurred) {
                try {
                    threadArg.wait();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }

        System.out.print("Result array (numbers with " + threadArg.K + " ones in decimal): ");
        for (int num : threadArg.resultArray) {
            System.out.print(num + " ");
        }
        System.out.println();

        synchronized (threadArg.cs) {
            System.out.println("Non-zero count: " + threadArg.nonZeroCount);
        }
    }
}