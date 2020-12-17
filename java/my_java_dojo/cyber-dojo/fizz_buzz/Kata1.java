/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package tw.idv.ken.kata.fizzbuzz;

/**
 *
 * @author ken
 */
/*
 * Multiples of 3: Fizz; multiples of 5: Buzz; Multiples of 15: FizzBuzz
*/
public class Kata1 {
    
    public String says(int x) {
        StringBuilder sb = new StringBuilder();
        if (x % 3 == 0) {
            sb.append("Fizz");
        } 

        if (x % 5 == 0) {
            sb.append("Buzz");
        }

        if(sb.length() == 0) {
            sb.append(x);
        }
        return sb.toString();           
    }
}
