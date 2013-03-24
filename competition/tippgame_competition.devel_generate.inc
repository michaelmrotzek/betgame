<?php 

/**
 */
function tippgame_competition_devel_generate_content_form($form, &$form_state) {
  $form['matchplan'] = array(
    '#type' => 'select',
    '#title' => t('Matchplan generated competitions are based on'),
    '#options' => _tippgame_matchplan_select_matchplan(),
  );
  $form['num'] = array(
    '#type' => 'textfield',
    '#title' => t('How many competitions would you like to generate?'),
    '#default_value' => 1,
    '#size' => 10,
    '#required' => TRUE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Generate'),
  );
  return $form;
}

function tippgame_competition_devel_generate_content_form_validate($form_id, &$form_state) {
  $values = $form_state['values'];
  if($values['matchplan'] <= 0) {
    form_set_error('maptchplan', t('Please choose a matchplan'));
  }
  if($values['num'] <= 0) {
    form_set_error('num', t('Amount of competitions to generate must be at least 1'));
  }
}
/**
 * FormAPI submission to generate nodes.
 */
function tippgame_competition_devel_generate_content_form_submit($form_id, &$form_state) {
  $values = $form_state['values'];
  $num = $values['num'];
  if($values['matchplan'] > 0 && $num > 0) {
    tippgame_competition_devel_generate_competition_add($values['matchplan'], $num);

    drupal_set_message(t('Competitions generated'));
  }
}

function tippgame_competition_devel_generate_competition_add($matchplan_id, $num) {
  module_load_include('inc', 'devel_generate');

  for($i = 0; $i < $num; $i++) {
    $c = new Competition();
    $c->setName(devel_generate_word(mt_rand(6, 30)));
    $c->setMatchplan($matchplan_id);

    $competition_id = tippgame_competition_dao()->storeCompetition($c);

    tippgame_competition_controller()->autoCreateGroupsTeams($competition_id);
    tippgame_competition_controller()->generateMatches($competition_id);

    $matches = tippgame_competition_dao()->findMatchs($competition_id);
    foreach($matches as $match) {
      $home = mt_rand(0, 12);
      $guest = mt_rand(0, 12);
      tippgame_competition_controller()->setResult($match, $home, $guest);

      if($match->getReturnmatchgamenumber() && ($returnmatch = tippgame_competition_dao()->findMatchByGamenumber($competition_id, $match->getReturnmatchgamenumber()))) {
        $home = mt_rand(0, 12);
        $guest = mt_rand(0, 12);
        tippgame_competition_controller()->setResult($returnmatch, $home, $guest);

      }

    }

  }

}