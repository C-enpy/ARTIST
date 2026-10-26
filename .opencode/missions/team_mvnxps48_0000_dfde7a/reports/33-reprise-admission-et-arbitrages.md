# 33 — Admission QA validée ; arbitrages B3 et C2

2026-10-25. Objectif : terminer le scope initial B1–B6, sans reprendre les campagnes acquises ni clore les gaps par silence. Étape : amendement QA conditionnel et décisions avant toute calibration ou correction Reader.

## Depuis le rapport32

La restitution post-restriction de pool-6 a été reçue par team_results : lecture canonique32 réussie, émission team_message confirmée, catalogue Code Mode limité à team_message. Le worker attribue correctement PATCH/GET au contrôle du lead ; aucune prétention de sandbox. Le lead a validé cette admission et envoyé le GO de fond dans la même session pour la proposition QA uniquement. Aucun nouveau worker, droit, code, build ou test lancé lors de cette reprise.

## Preuves et inconnues conservées

- B1 : rapport30 et manifeste portable docs/_meta/session-2026-10-25-b1-causal-validation.json ; deux mécanismes historiques négatifs isolés, candidat actuel vert sur les réductions. Pas une preuve exhaustive des permutations ; revue finale attendue.
- B3 : contrat architecte proposé, non adopté. Calibration stdio non exécutée ; catch authentique et close réellement échoué restent non prouvés. Aucun correctif autorisé à ce stade.
- C2 : rapport28, défaut de préparation de fixture contemporain retrouvé ; explication forte pour Empty, probable pour MultiBlock. XML primaires, valeurs complètes MultiBlock et identité binaire pré-run restent absents. Pas de clôture acquise.
- Admission QA : restriction native et transport vérifiés (rapport32 + remise post-restriction), pas correction durable de l'overlay du plugin ni garantie de sandbox.

## Options, compromis et recommandation

1. **B3 : calibrer stdio et rechercher un close authentiquement échoué de façon bornée**, sans modification Reader ni abandon d'obligation. Recommandé : tester d'abord si la primitive conserve réellement l'erreur et les modes texte/chemins. Alternative Win32 directe : information OS explicite, mais surface de compatibilité plus importante. Plan QA final commité et contrat de sonde avant auteur ; runner lead. L'adoption du futur Reader et un éventuel remplacement du catch feront l'objet d'une décision ultérieure.
2. **C2 : accepter une fermeture administrative motivée, bornée par les manques ci-dessus.** Recommandé sur la provenance positive retrouvée, sans déclarer causalité exhaustive ou moteur corrigé ; réouverture sur primaire contradictoire ou nouveau symptôme identifié. Alternative : maintien OPEN/INCONCLUSIVE sur les primaires manquants, sans chasse identique ni replay actuel prétendant reconstruire le passé.

## Décisions exactes demandées

- Autoriser calibration stdio + recherche OS close bornées, ou privilégier l'étude Win32 directe ? Aucune de ces options n'autorise encore une correction Reader ou une substitution catch/close.
- Accepter la fermeture motivée bornée de B5/C2, ou maintenir OPEN/INCONCLUSIVE sur les primaires nommés ?

L'amendement documentaire QA peut avancer pendant ces décisions. Aucun PASS global, changement ADR, nouvelle campagne ASan/GPU, installation ou backend implicite.
