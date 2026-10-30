# 24 — Défaut C++ confirmé dans le producteur CPU

## Problème et conséquence

La revue indépendante a trouvé un type incomplet dans la déclaration du vecteur de miroirs CPU : le code emploie `UniformContext` sans fournir le paramètre de template exigé. Le coordinateur a recoupé le constat avec le producteur et la déclaration réelle. Le programme est mal formé ; sa réception est bloquée avant qualification. Aucun compilateur n'a été lancé pour établir ce constat statique.

Le défaut est retourné au propriétaire. Il peut corriger la spécialisation locale après sa tranche Python actuelle, sans changer l'API moteur, inventer un backend ni affaiblir les tests. Un ré-audit du diff sera requis. Les tests synthétiques acquis restent valides et les autres branches peuvent continuer.

## Risque distinct à résoudre

Le parseur natif suppose des arguments UTF-8 et construit le chemin résultat depuis une chaîne étroite. Sous Windows avec page ANSI différente, un chemin contenant des caractères non ASCII pourrait être rejeté ou mal converti. C'est un risque statique, pas une panne reproduite. Le propriétaire doit examiner le contrat CRT/manifeste existant et proposer un transport Unicode cohérent, sans restreindre silencieusement l'interface à ASCII.

## Revue et limites

La revue a aussi examiné les bornes numériques, la création exclusive du résultat, l'échappement JSON, l'ordre des durées de vie GL et la structure FBO/oracles. Elle ne constitue pas une preuve exhaustive, compilée ou graphique. La confirmation géométrique des points pixels est renvoyée à une revue shader distincte avant réception sensible.

## Annexe technique

- Revue : correctness-reviewer, pool-4, candidat natif `result_mvu7hkm1_000m_zkrp5n`, message `msg_18a1b032e001ZZmKxUJBV07oAG`.
- Sources examinées inchangées entre `60f32c52` et les HEAD de revue `57e47178` / `1834af6f`.
- Défaut : `ARTIST/verification/CpuUniformWorkload.cpp:12` ; déclaration `ARTIST/include/graphic/context/UniformContext.hpp:13`.
- Risque Windows : `ARTIST/verification/WorkloadProtocol.hpp:94–118`.
- Serena canonique ne reflète pas le worktree : contrôle des fichiers réels, pas de diagnostic LSP du candidat.
- Aucun test, build, rendu, benchmark ou édition par le reviewer ; aucune compilation par le coordinateur.
