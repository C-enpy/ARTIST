# 18 — Validation intégrée normale et ASan recoupée, audit final à recevoir

## Résultat

Le runner lead a exécuté les cinq contrôles convenus sur le candidat intégré
avec le complément QA : **579 tests normaux et 579 ASan passants**, chacun
504 CPU + 75 OpenGL, zéro failure/error/skip. Découvertes normales/ASan et XML
ont les mêmes 579 noms uniques. Les deux configurations ont été reconstruites
entièrement pour éviter le défaut d'incrémentalité observé.

Le candidat satisfait ces critères exécutés Windows Debug. L'audit fresh de la
capacité des tests à détecter les bugs reste à recevoir avant la clôture, ainsi
que la photographie documentaire finale. Aucun gain de performance revendiqué.

## Preuves finales du lead

Commandes réellement exécutées, chacune exit0 :
1. powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1
   -Configuration Debug -Jobs 1 -TestLabel unit : 504 CPU.
2. ctest --test-dir out/build/windows-msvc-debug -N : 579 découverts.
3. ctest --test-dir out/build/windows-msvc-debug -L unit --output-on-failure
   --no-tests=error -j 1 : 504 CPU.
4. Même commande label opengl : 75 OpenGL.
5. powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1
   -Configuration Debug -Jobs 1 -AddressSanitizer : 579.

CTest de run ajoute seulement --output-junit vers un fichier unique. CTest
utilisé : C:/Program Files/CMake/bin/ctest.exe. Découvertes JSON supplémentaires,
canari ASan 0/86 et absence de marqueur d'erreur ASan tardif dans les logs finaux
recoupés. detect_leaks=0 : aucune preuve de fuite. Les expérimentations rouges
et artefacts périmés restent séparés du final, comme documenté au rapport17.

## Identité et limites

Source finale `3d0e1452cd9bd7b126b5fd1e43c73a5d0b1b6e23`, quinze empreintes
source vérifiées, cinq binaires/probe empreintés, aucun delta code local.
Les headers de production sont ceux de la revue indépendante précédente ; les
ajouts depuis H2 sont uniquement deux tests et trois assertions QA. Les fichiers
étrangers préexistants et leur renommage stagé restent hors commit.

Les limites TDD historiques restent explicites, pas reconstruites par les mutants.
Release/Linux/autres API, retraite GPU générale et performances ne sont pas testés.
Le défaut de dépendances d'en-tête Ninja observé est contourné par clean-first,
pas corrigé dans cette mission. Aucun changement de harness ou toolchain livré.

## Suite

Publier l'audit test-reviewer fresh HORS board d'implémentation, uniquement avec
contrat/candidat/preuves neutres et permissions restrictives vérifiées. Runner
reste le lead ; tout finding revient au propriétaire et au ré-audit. Après retour
accepté, mettre à jour ADR-0004/registre/usage/état courant/prochaine mission à
la seule portée prouvée, sans nouveau GO de jalon.

## Annexe

- Archives lead `out/qa-prepared-float/lead-runner-20261027/`, préfixes finaux10–18.
- Manifeste `final-validation.json`, SHA256
  `312642425d7fd039a0469a15be373fde7a55f651b3a6bf5b7b6a0b2261eb048c`.
- [Entrées neutres d'audit](../final-audit-input.md),
  [expériences et exclusions](17-controles-discriminants-et-normal-integre.md).
