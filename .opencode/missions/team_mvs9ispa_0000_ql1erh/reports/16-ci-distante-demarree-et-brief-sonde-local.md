# QA Linux — CI distante démarrée, contrat de sonde remis sur disque

## Progression observée

La reproduction distante a réellement démarré sur une branche dédiée, sans modifier la branche principale. Le lead a vérifié l'API GitHub : les deux jobs, normal et ASan, sont en cours de construction. **Aucun résultat distant final n'est encore acquis.**

Les résultats locaux restent ceux du rapport15 : mêmes581tests normal/ASan, trois échecs d'attributs conservés, aucun skip. Le lancement CI ne corrige pas ces échecs et ne vaut pas PASS.

## Transmission de la sonde réparée sans élargissement

L'auteur a signalé ne pas avoir reçu les briefs de sonde précédemment envoyés. Le lead ne traite pas ce défaut de livraison comme une autorisation manquante ni comme une raison de relancer un worker. Il a persisté le contrat complet directement dans le worktree de l'auteur, puis transmis son chemin court et absolu.

Ce contrat réserve explicitement une petite sonde temporaire autonome : scalaire, array1 et array2, requêtes de noms/éléments et comparaison moderne conditionnelle. Pas de modification du moteur, des fixtures requises ou des assertions ; outils existants seulement. Le fichier temporaire est exclu du livrable de production. La lecture effective et l'exécution restent à confirmer ; sa présence seule ne prouve pas leur réalisation.

La sonde Linux peut être exécutée pendant les jobs distants : ressources distinctes, sans construction concurrente dans le même arbre local.

## Relecture ciblée des helpers, avant intégration

Le commit distant initial contient le workflow et le helper compagnon, réutilisant lanceur/presets, canari existant et bibliothèque standard CTestJSON/XML. Le workflow utilise deux runners hébergés distincts, impose llvmpipe, garde les artefacts même en échec et ne dépend plus de l'action Sonar introuvable pour construire la baseline.

La correction locale du helper donne désormais FAIL et conserve les comptes graphiques après échecCTest. Cinq tests sont annoncés réussis par l'auteur sous Windows et Linux, non encore exécutés par le lead. Le lead a lu le code et détecté une limite précise : le cinquième test vérifie le résumé75cas/3échecs/code8, mais pas encore la sortie réelle du contrôleur vers manifestFAIL et code8. Un complément ciblé a été demandé dans les mêmes ownership et périmètre, sans campagne moteur ni nouveau framework.

L'auteur ne pousse pas immédiatement cette correction afin de ne pas déclencher un second run aveugle pendant le premier. L'archive initiale INCONCLUSIVE reste intacte ; la correction ne lui attribuera pas rétroactivement un autre contenu. L'intégration finale et les preuves du candidat corrigé restent à vérifier.

## Suite

Recueillir le résultat de sonde et le résultat distant, intégrer workflow/helpers après leur remise, puis libérer les contrôles de gaps et l'audit selon leurs préconditions effectives. Aucun nouveau GO de jalon n'est demandé. Tout changement de contrat public ou de provenance de métadonnées exige encore l'avis d'architecture et la décision appropriée avant auteur.

## Annexe de traçabilité

- Run vérifié : [Linux Debug parity sur GitHub](https://github.com/C-enpy/ARTIST/actions/runs/40741589259).
- Branche : `mission/linux-qa-parity`, événement push ; commit `84bacef5fcf3f13fad5b9b29e18eeaa70b72f911`.
- Deux jobs observés in_progress, étape « Build and validate the required population » : baseline(normal), baseline(asan).
- Vérification lead : `gh run view 40741589259 --repo C-enpy/ARTIST --json ...`, lecture git du commit/workflow, lectures ciblées du helper et tests dans le worktree auteur. Aucun test ou build relancé.
- Brief local : worktree pool-5, `.tmp/lead-attribute-probe-contract.md` ; source temporaire Linux réservée `.tmp/qa-attribute-name-probe.cpp`, sorties neuves sous `out/linux-evidence/`.
- Preuves locales déjà copiées par l'auteur dans son worktree : `out/linux-parity-evidence/linux-evidence/<runid>/` ; copies non encore toutes recoupées par le lead.
