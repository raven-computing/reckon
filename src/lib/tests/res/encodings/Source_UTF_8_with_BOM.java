/** This source file is encoded with UTF-8 with a BOM at the beginning **/

public class Source_UTF_8_with_BOM {

    public static void main(String[] args) {
        System.out.println("Some text with 😀 and 🙂 !");
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
