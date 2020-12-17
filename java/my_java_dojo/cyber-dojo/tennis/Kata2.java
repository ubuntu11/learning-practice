/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package tw.idv.ken.kata.tennis;

/**
 * Tennns Score, see http://www.tennistips.org/tennis-scoring.html for detailed
 * rules.
 * @author ken
 */
public class Kata2 {
    private String[] gamePoints = new String[] {"Love", "Fifteen", "Thirty", "Forty"};
    String announce(int server, int receiver) {
        if (server == receiver) {
            if (server >= 3) {
                return "Deuce";
            } else {
                return gamePoints[server] + " All";
            }
        }
        if (server  == 4 || receiver >= 4) {//think: why server is not >= 4
            if (Math.abs(server - receiver) >= 2) {
                return "Game Over";
            }
            return "Advantage " + ((server > receiver) ? "In" : "Out");
        }
        return gamePoints[server] + " " + gamePoints[receiver];
    }
    
}
