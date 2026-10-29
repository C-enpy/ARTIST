# Fusion vers main — périmètre supplémentaire à confirmer

## Demande et constat

L'utilisateur demande de fusionner la PR nº3 dans main puis de récupérer main
localement. La PR est ouverte en brouillon et fusionnable, mais cible actuellement
la branche de référence de début de mission, pas main.

Le contrôle confirme que main distant n'a toujours pas les **60 commits antérieurs
à la mission Linux**. Ils contiennent notamment le nettoyage du Reader de shaders
et les uniforms float préparés, plus leurs tests et documents. Cibler main ferait
passer la PR de **51 à 205 fichiers** et publierait donc aussi ces livraisons.
Il n'existe pas d'autre PR ouverte permettant de les intégrer séparément.

Les campagnes finales ont bien exécuté l'état combiné contenant ces prérequis ;
ce constat ne constitue cependant pas une autorisation implicite de publier sur
main les fonctionnalités supplémentaires d'une PR présentée comme limitée à Linux.
Aucune cible de PR, branche distante ou protection n'est modifiée à ce stade.

## Recommandation et choix

**Recommandation : confirmer l'inclusion des prérequis**, puis cibler main,
sortir la PR du brouillon, vérifier les contrôles applicables et fusionner sans
forcer ni contourner de protection. Cela publie le même état combiné déjà testé,
mais avec un périmètre de revue plus large que les seuls changements Linux.
Ensuite mettre main local à jour par avance rapide si possible, en préservant
les modifications non commitées ; aucun reset/clean/stash implicite.

**Reporter la fusion** conserve la PR et main distant inchangés. L'intégration
Linux attend alors la publication séparée de ces prérequis. Isoler Linux sur le
main actuel sans eux nécessiterait une adaptation et une nouvelle qualification,
pas une simple commande de fusion. Aucune durée n'est estimée.

La décision requise porte uniquement sur la publication des prérequis, pas sur
le droit de fusionner la PR déjà demandé par l'utilisateur.

## Annexe technique

Décision utilisateur reçue : **Fusionner prérequis et Linux**. L'inclusion des
60 commits antérieurs est explicitement autorisée. Le lead peut donc cibler main,
vérifier les contrôles applicables et fusionner, puis récupérer main localement.
Cette décision ne permet ni de contourner une protection ni d'écraser le diff local.

- PR : https://github.com/C-enpy/ARTIST/pull/3, draft, MERGEABLE/CLEAN sur sa base
  actuelle ; aucun contrôle attaché à la PR empilée.
- Main distant : `b7cc8848ca730d4c060ffad68e82de2144eb0785`.
- Base de mission : `bcd3857494b361fab079092a9880b35522130022`, 60 commits après main.
- Source PR et HEAD local : `191bcca6423c499849ebaa914bdfb4c3b592931d`.
- Préflight : `out/logs/mission-linux-qa/merge-pr3-preflight.json`.
- Commandes : git fetch origin ; gh pr view/list ; rev-list/log/diff comparant
  main, base de mission et source. Modifications locales préexistantes toujours
  présentes et non intégrées aux commits de mission.
