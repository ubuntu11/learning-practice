
public class ScoreCard {
    private String[] scores;

    public String[] getScores() {
        return this.scores;
    }

    public static int scoreText2Int(String scoreText) {
        if (scoreText.equalsIgnoreCase("X") || scoreText.indexOf("/")>0) {
            return 10;
        } else {
            return Integer.parseInt(scoreText.substring(0,1)) +
                Integer.parseInt(scoreText.substring(1,2));
        }
    }

    public int computeFinalScore() {
        int finalScore = 0;
        for(int i=0;i<10;i++) {
            finalScore += scoreText2Int(this.scores[i]) + computeBonus(i + 1);
        }

        return finalScore;
    }

    /**
     *
     * param currentTurn: start from 1
    **/
    public int computeBonus(int currentTurn) {
        int bonus = 0;
        if (this.scores[currentTurn - 1].equalsIgnoreCase("X")) {
            bonus += getScoresOfNext2Balls(currentTurn);
        } else if(this.scores[currentTurn - 1].indexOf("/")>0) {
            bonus += getScoresOfNextBall(currentTurn);
        }

        return bonus;
    }

    /**
     *
     * param currentTurn: start from 1
    **/
    public int getScoresOfNext2Balls(int currentTurn) {
        int sum = 0;
        String scoreTextOfNextTurn = this.scores[currentTurn];
        if (scoreTextOfNextTurn.equalsIgnoreCase("X")){
           String scoreTextOfNext2Turn = this.scores[currentTurn + 1];
           if (scoreTextOfNext2Turn.equalsIgnoreCase("X")) {
               sum += 10;
           } else {
               sum += Integer.parseInt(scoreTextOfNext2Turn.substring(0,1));
           }
        }
        sum += ScoreCard.scoreText2Int(scoreTextOfNextTurn);
        return sum;
    }

    public int getScoresOfNextBall(int currentTurn) {
        String scoreTextOfNextTurn = this.scores[currentTurn];
        if (scoreTextOfNextTurn.equalsIgnoreCase("X")){
            return 10;
        } else {
            return Integer.parseInt(scoreTextOfNextTurn.substring(0,1));
        }
    }

    public ScoreCard(String... scoreStrings) {
        this.scores = scoreStrings;
    }
}
