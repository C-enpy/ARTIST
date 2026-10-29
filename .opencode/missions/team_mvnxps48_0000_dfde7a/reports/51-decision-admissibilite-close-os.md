# 51 — Admissibilité explicite d'une calibration close par politique OS

2026-10-26. Étape : auteur S_CODE_RED admis/isolation prouvée, source attendue ; étude C-OS terminée et intégrée au50. **Aucun build/test/flag/probe OS exécuté.** Les deux sujets restent indépendants.

## Résultat documentaire nouveau

Refs Microsoft recoupées lead47/50 : HANDLE_FLAG_PROTECT_FROM_CLOSE refuse la fermeture par CloseHandle d'un handle qui reste valide ; sourceCRT42 compatible avec propagation vers fcloseEOF/consommationFILE*, mais DLLchargée et retour/exception précis non encore mesurés. _get_osfhandle ne transfère pas ownership : retraitflag+CloseHandle manuel interdit comme cleanup supposé sûr.

Complément ExitProcess lu par lead et architecte : tous les handles d'un **enfant possédé dédié** sont récupérés avant signal de terminaison ; retour main mène à cette sortie. Cette frontière permet documentairement de ne jamais refermer le handle emprunté : enfant seul owner, une fclose, plus d'usageFILE/fd ; parent attend fin puis contrôle ses propres ressources/fixtures/voisins. DeadlockDLLdetach reste risque : timeout n'est pas preuve de fin, aucun TerminateProcess fallback adopté, aucun nettoyage avant terminaison constatée.

La condition serait donc **un refus natif délibérément injecté par politique OS**, pas une panne spontanée de stockage. Ce n'est pas un throw/flagEOF/ferror écrit par mock ; ce n'est pas encore une preuve exécutée. L'étude46 seule n'autorisait pas cette injection.

## Options et recommandation

**A — Admettre cette injection pour une calibration FILE* séparée**, recommandé pour vérifier la piste concrète plutôt que s'arrêter au documentaire. Autoriser ciblément la protection du seul handle valide d'un probe enfant possédé, extraction d'emprunt par_get_osfhandle pour cette instrumentation uniquement ; aucune seam/accès au ifstream privé du Reader, aucun code moteur modifié. Protocole et QA doivent définir contrôle nominal/fault/recoveryprocess, identitéCRTchargée, oraclesSyscall/CRT distincts, supervision/timeout/cleanup ; commis avant auteur isolé et commandes exactes approuvées avant runner lead. Tout code est développement≥70 avec contrats et TDD/characterisation honnêtement séparés. Source/outputs distincts de S, pas de second écrivain dans son sous-arbre ni compilation concurrente.

Limite essentielle : une calibration réussie démontrerait **seulement fermeture native/CRT d'un FILE*contrôlé**. Elle ne remplirait pas à elle seule l'obligation non-publicationReader ni le catchifstream ; leur traitement et migration demeurent décisions distinctes. Ne pas renommer cette preuve partielle «M5couvert». Pas codeOS/errno prédéterminé ou assertion relâchée si surprise.

**B — Accepter une disposition résiduelle motivée M5 non couvert**, sans cette expérience : absence de preuve explicite, impact publication/lifetime, propriétaire lead+architecte, réouverture sur dispositif admissible ou changement pertinent. Cette acceptation ne vaut pas fermeture testée, pas impossibilité générale ni nouveau PASS moteur. Elle doit être explicite, pas un DEFER automatique.

## Décision exacte

Admettre cette injection OS ciblée pour la seule calibration FILE* sous les gates ci-dessus, ou accepter la disposition résiduelle motivée M5 non couvert ? Aucun changement du contrat Reader/catch/messages/modes, politique modèle ou permissions générales dans l'un ou l'autre choix.
