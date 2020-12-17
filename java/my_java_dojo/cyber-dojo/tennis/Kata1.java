/**
 * 
 */
package tw.idv.ken.kata.tennis;

import java.util.Arrays;

/**
 * @author ken
 * 根據兩個player的得分，顯示出正確的網球比分讀數.
 * 
 * 0 	"love"
 * 1 	"15"
 * 2 	"30"
 * 3 	"40"
 * 4 	"game"
 * (1)When players are tied by one or two points, the score is described as 
 * "15-all" and "30-all"
 * (2)If each player has won three points, the score is described as "deuce" rather 
 * than "40–all".From this point on, whenever the score is tied, it is described 
 * as "deuce" regardless of how many points have been played.
 * (3)The player who wins the next point after deuce is said to have the advantage.
 */
public class Kata1 {
	String[] points = (String[]) Arrays.asList("Love", "Fifteen", "Thirty",
			"Forty").toArray();

	public Object scores(int playerA, int playerB) {
		if (playerA != playerB) {
			if (playerA >= 4 || playerB >= 4) {
				return whoLeadTheGame(playerA, playerB) + " "
						+ whatKindOfLead(playerA, playerB);
			}
			return points[playerA] + " " + points[playerB];
		} else {
			if (playerA >= 3)
				return "Deuce";
			else
				return points[playerA] + " All";
		}
	}
	
	private String whoLeadTheGame(int playerA, int playerB) {
		if (playerA > playerB) {
			return "A";
		} else {
			return "B";
		}
	}
	
	private String whatKindOfLead(int playerA, int playerB) {
		if (Math.abs(playerA - playerB) > 1) {
			return "Win";
		} else {
			return "Advantage";
		}
	}

}
