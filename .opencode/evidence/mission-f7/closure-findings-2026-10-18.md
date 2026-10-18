# F7 — findings avant clôture (2026-10-18)

État : clôture retenue, pas de PASS global. Candidat moteur `18dcf31`,
barrière historique `17019e4`, tests TE intégrés `1dd7cd1`, correction test
T1-1 du lead `176d167`. Les résultats historiques ne sont pas une preuve des
corrections encore à venir.

## F1 — diagnostic AttributeContext

Le texte du manifeste T4 qui déclarait le mismatch inatteignable était faux.
Le changement du type stocké reste susceptible de lever SET::TYPE_MISMATCH ;
`typeid(m_value)` décrit std::any, pas son contenu. Correction documentaire
appliquée au manifeste, JSON parsé via Context Mode. Aucun correctif du message
C++ exécuté : le défaut diagnostique reste ouvert, pas résolu par la documentation.

## F2 — cardinalité des uploads d'uniforms

Finding fresh T5 : le Reader T3 publie désormais la taille réfléchie N, tandis
que les setters vector/matrix peuvent fournir une seule valeur à glUniform* avec
count=N. Risque de lecture hors limites si N>1. Ne pas traiter ce défaut comme
inatteignable ni le clore par report implicite à ADR-0004 (toujours Proposed).
Consultation lecture seule `f2-architect` lancée (C60/R65 PM/LLM, fresh, sans
worktree), en parallèle du ré-audit T1-1. Contrat minimal et RED sûr requis avant
implémentation ; ne pas provoquer volontairement un accès driver hors limites.

## T1-1 — oracle de lifetime des gap-tests

Proposition initiale du test-reviewer : LifeProbe avec compteur de destructions.
Exécutant : lead ; premier essai deaths==0 échoue parce que l'ancien payload de
destination est détruit au move, conformément au contrat. Premier FAIL conservé
dans `out/mission-f7/te/t1-1-patch.junit.xml`.

Correction lead `176d167` : deaths==1, puis suite native 271/271 (226 unit +45 GL),
zéro skip rapporté ; preuves `t1-1-v2.junit.xml` et `after-t11-v2.junit.xml`.
Ces succès ne clôturent pas le finding : ré-audit indépendant requis, notamment
pour le mutant combinant fuite ancien payload et destruction du transféré, et
pour l'absence d'accès après détection d'un lifetime invalide (ASSERT avant lecture).
Option soumise au reviewer : compteurs séparés pour les deux payloads.

## Limites de preuve

- Aucune nouvelle campagne ASan n'est revendiquée pour les tests TE/T1-1.
- L'identité des trois TU TE auteur `f8dc61d` / intégration `1dd7cd1` a été
  attestée par diff Git vide ; elle ne remplace pas l'exécution du nouveau patch.
- Le diagnostic précédent « profil QA mort/inert » n'est pas établi par un
  transcript natif : worktree vierge et absence de message ne prouvent pas la cause.
- Les règles QA restent inchangées ; aucune autorisation ajoutée ou présumée.
