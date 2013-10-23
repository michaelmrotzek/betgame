<?php 

interface BetScoreRule {
  
  
  public function calculateScore(Match $match, Prediction $prediction);
  
}