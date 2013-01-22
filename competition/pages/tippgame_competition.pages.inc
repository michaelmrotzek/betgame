<?php 

function tippgame_competition_page_competition(Competition $competition) {
  $ra = array();

  if(!$competition) {
    drupal_set_message(t('No valid competition'), 'error');
    return $ra;
  }

  $groups = tippgame_competition_dao()->findGroups($competition->getId());
  if(!empty($groups)) {
    // only show if more than one group

    $ra['groups'] = array(
    );

    foreach($groups as $group) {
      $group_id = $group->getId();

      if(count($groups) > 1) {
        $ra['groups'][$group_id]['title'] = array(
          '#markup' => $group->getName(),
          '#prefix' => '<h2>',
          '#suffix' => '</h2>',
        );
      }

      $matches = tippgame_competition_dao()->findMatchsByGroup($group_id);
      $ra['groups'][$group_id]['matches'] = array(
        '#matches' => $matches,
        '#cols' => array(

        ),
        '#showHeader' => FALSE,
        '#useShortnames' => FALSE,
        '#theme' => 'tippgame_competition_matches',
      );

      $tabledata = tippgame_competition_controller()->calculateTable($group_id);
      $ra['groups'][$group_id]['table'] = array(
        '#tabledata' => $tabledata,
        '#useShortnames' => FALSE,
        '#theme' => 'tippgame_competition_scoretable',
      );

    }

  }

  // render knockout matches by gameday
  $knockoutgamedays = tippgame_competition_dao()->findGamedaysKnockoutPhase($competition->getId());
  if($knockoutgamedays) {
    foreach($knockoutgamedays as $gameday) {
      $gameday_id = $gameday->getId();
      $ra['knockout'][$gameday_id]['title'] = array(
        '#markup' => $gameday->getName(),
        '#prefix' => '<h2>',
        '#suffix' => '</h2>',
      );

      $knockoutmatches = tippgame_competition_dao()->findMatchsByGameday($gameday_id);
      if($knockoutmatches) {
        $ra['knockout'][$gameday_id]['matches'] = array(
          '#matches' => $knockoutmatches,
          '#cols' => array(

          ),
          '#showHeader' => FALSE,
          '#useShortnames' => FALSE,
          '#theme' => 'tippgame_competition_matches',
        );
      }
    }
  }


  return $ra;
}

/**
 * Returns HTML table representing the matches.
 *
 * @param $variables
 *   An associative array containing:
 *   - matches: List of matches
 *   - useShortnames: Boolean if short names instead of name should be rendered.
 *   - showHeader: Boolean
 *   - cols: array Shown columns, leave empty for default.
 */
function theme_tippgame_competition_matches(&$variables) {
  $output = '';

  $matches = $variables['matches'];
  $showHeader = $variables['showHeader'];
  $useShortnames = $variables['useShortnames'];
  $access= user_access('administer tippgame');

  if($matches && !empty($matches)) {
    foreach($matches as $match) {

      $group = tippgame_competition_dao()->findGroup($match->getGroupid());
      $groupval = '';
      if($group) {
        $groupval = l($useShortnames ? $group->getShortname() : $group->getName(), 'tippgame/competition/group/'.$group->getId());
      }
      $gd = tippgame_competition_dao()->findGameday($match->getGameday());
      $gamedayval = '';
      if($gd) {
        $gamedayval = l($gd->getName(), 'tippgame/competition/gameday/'.$gd->getId());
      }

      $fields = array(
        array('data' => $match->getKickoff() ? format_date($match->getKickoff()) : ''),
        array('data' => $match->getLocation() ? $match->getLocation() : ''),
        array('data' => $match->getGamenumber()),
        array('data' => $gamedayval),
        array('data' => $groupval),
        array('data' => theme('tippgame_competition_match_team', array('match' => $match, 'type' => 'home', 'useShortname' => $useShortnames))),
        array('data' => '-'),
        array('data' => theme('tippgame_competition_match_team', array('match' => $match, 'type' => 'guest', 'useShortname' => $useShortnames))),
        array('data' => theme('tippgame_competition_match_result', array('match' => $match))),
      );

      $tablerow = array(
        'data' => $fields,
        'class' => array('match')
      );

      $rows[] = $tablerow;
    }

  }

  $header = array();
  if($showHeader) {
    $header = array(
      t('Kickoff'),
      t('Location'),
      t('Game number'),
      t('Gameday'),
      t('Group'),
      t('Home'),
      '',
      t('Guest'),
      t('Result'),
    );
    if($access) {
      $header[] = '';
    }
  }

  $build = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#attributes' => array('id' => 'match-table'),
  );

  $output = drupal_render($build);
  return $output;

}

/**
 * Returns HTML table representing the matches.
 *
 * @param $variables
 *   An associative array containing:
 *   - tabledata: List of tabledata rows
 *   - useShortnames: Boolean if short names instead of name should be rendered.
 *   - showHeader: Boolean
 *   - cols: array Shown columns, leave empty for default.
 */
function theme_tippgame_competition_scoretable(&$variables) {
  $output = '';

  $tablerows = $variables['tabledata'];
  $showHeader = $variables['showHeader'];
  $useShortnames = $variables['useShortnames'];

  if($tablerows && !empty($tablerows)) {
    foreach($tablerows as $table) {

      $team = tippgame_competition_dao()->findTeam($table->team);
      $diff = $table->goalsown - $table->goalsagainst;
      $goals = $table->goalsown . ':' . $table->goalsagainst;

      $fields = array(
        array('data' => $table->position . '.'),
        array('data' => theme('tippgame_competition_team', array('team' => $team, 'useShortname' => $useShortnames))),
        array('data' => $table->matchesplayed),
        array('data' => $goals),
        array('data' => $diff),
        array('data' => $table->points),
      );

      $tablerow = array(
        'data' => $fields,
        'class' => array('match')
      );

      $rows[] = $tablerow;
    }

  }

  $header = array();
  if($showHeader) {
    $header = array(
      t(''),
      t('Team'),
      t('Played'),
      t('Goals'),
      t('Diff'),
      t('Points'),
    );
  }

  $build = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#attributes' => array('id' => 'score-table'),
  );

  $output = drupal_render($build);
  return $output;

}

/**
 * Team name.
 *
 * @param $variables
 *   An associative array containing:
 *   - match: Match
 *   - type: String home|guest
 *   - useShortname: Boolean if short name to use instead of full name.
 */
function theme_tippgame_competition_match_team($variables) {
  $output = '';

  $match = $variables['match'];
  if($match) {
    $type = $variables['type'];
    $useShortname = $variables['useShortname'];

    if($match->isPlaceholder()) {
      $placeholder = NULL;
      if($type == 'guest') {
        $placeholder = unserialize($match->getPlaceholderguestval());
      } else {
        $placeholder = unserialize($match->getPlaceholderhomeval());
      }
      $output .= theme('tippgame_competition_team_placeholder', array('placeholder' => $placeholder, 'competition_id' => $match->getCompetition(), 'useShortname' => $useShortname));

    } else {
      $team_id = 0;
      if($type == 'guest') {
        $team_id = $match->getGuestteam();
      } else {
        $team_id = $match->getHometeam();
      }

      if($team = tippgame_competition_dao()->findTeam($team_id)) {
        $output .= theme('tippgame_competition_team', array('team' => $team, 'useShortname' => $useShortname));

      }
    }

  }

  return $output;
}

/**
 * Team name.
 *
 * @param $variables
 *   An associative array containing:
 *   - team: Match
 *   - useShortname: Boolean if short name to use instead of full name.
 */
function theme_tippgame_competition_team($variables) {
  $output = '';

  $useShortname = $variables['useShortname'];
  $team = $variables['team'];
  if($team) {
    $output .= $useShortname ? $team->getShortname() : $team->getName();
  }

  return $output;
}

/**
 * Render team placeholder of a match.
 *
 * @param $variables
 *   An associative array containing:
 *   - competition_id: Competition id
 *   - placeholder: PlaceholderGroupResult|PlaceholderGameResult
 *   - useShortname: Boolean if short name to use instead of full name.
 */
function theme_tippgame_competition_team_placeholder($variables) {
  global $language;

  $output = '';

  $placeholder = $variables['placeholder'];
  $useShortname = $variables['useShortname'];
  $competition_id = $variables['competition_id'];

  if($placeholder) {

    if ($placeholder instanceof PlaceholderGroupResult) {
      $group = tippgame_competition_controller()->getGroupByIndex($competition_id, $placeholder->groupindex);

      $output .= t('@position @group', array('@position' => $placeholder->groupposition.'.', '@group' => ($useShortname ? $group->getShortname() : $group->getName())));

    }
    if ($placeholder instanceof PlaceholderGameResult) {
      $output .= t('Winner game @gamenumber', array('@gamenumber' => $placeholder->gamenumber));
    }

  }
  return $output;
}

/**
 * Match result.
 *
 * @param $variables
 *   An associative array containing:
 *   - match: Match
 *   - showDetails: Boolean if detail of result will be rendered.
 */
function theme_tippgame_competition_match_result($variables) {
  $output = '';

  $match = $variables['match'];
  if($match) {
    $showDetails = $variables['showDetails'];

    if($result = tippgame_competition_dao()->findMatchResult($match->getId())) {
      $output .= $result->getHome() . '-' . $result->getGuest();
      if($showDetails) {
        switch ($result->getType()) {
          case MATCHRESULT_TYPE_EXTRATIME:
            $output .= ' ' . t('ET');
            break;

          case MATCHRESULT_TYPE_PENALTY:
            $output .= ' ' . t('P');
            break;

          default:
            ;
            break;
        }
      }

    } else {
      $output .= '-:-';
    }
  }
  return $output;
}


