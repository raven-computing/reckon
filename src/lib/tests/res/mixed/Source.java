public class Source {

    public static void main(String[] args) {
        System.out.println("Some text");
        boolean b = false;
        int i = 42;
        switch(i) {
            case 0:
                break;
            case 1:
                b = true;
                break;
            default:
                ++i;
                break;
        }
        System.out.println(i);
        System.out.println(String.valueOf(b));
    }

}
