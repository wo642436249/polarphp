// This source file is part of the polarphp.org open source project
//
// Copyright (c) 2017 - 2019 polarphp software foundation
// Copyright (c) 2017 - 2019 zzu_softboy <zzu_softboy@163.com>
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See https://polarphp.org/LICENSE.txt for license information
// See https://polarphp.org/CONTRIBUTORS.txt for the list of polarphp project authors
//
// Created by polarboy on 2019/05/17.

#include "polarphp/syntax/syntaxnode/ExprSyntaxNodes.h"

namespace polar::syntax {

///
/// NullExprSyntax
///
void NullExprSyntax::validate()
{
#ifdef POLAR_DEBUG_BUILD
   RefCountPtr<RawSyntax> raw = getRaw();
   if (isMissing()) {
      return;
   }
   assert(raw->getLayout().size() == NullExprSyntax::CHILDREN_COUNT);
#endif
}

TokenSyntax NullExprSyntax::getNullKeyword()
{
   return TokenSyntax{m_root, m_data->getChild(Cursor::NullKeyword).get()};
}

NullExprSyntax NullExprSyntax::withNullKeyword(std::optional<TokenSyntax> keyword)
{
   RefCountPtr<RawSyntax> rawKeyword;
   if (keyword.has_value()) {
      rawKeyword = keyword->getRaw();
   } else {
      rawKeyword = RawSyntax::missing(TokenKindType::T_NULL,
                                      OwnedString::makeUnowned((get_token_text(TokenKindType::T_NULL))));
   }
   return m_data->replaceChild<NullExprSyntax>(rawKeyword, Cursor::NullKeyword);
}

///
/// BraceDecoratedExprClauseSyntax
///
void BraceDecoratedExprClauseSyntax::validate()
{
#ifdef POLAR_DEBUG_BUILD
   RefCountPtr<RawSyntax> raw = getRaw();
   if (isMissing()) {
      return;
   }
   assert(raw->getLayout().size() == BraceDecoratedExprClauseSyntax::CHILDREN_COUNT);
   syntax_assert_child_token(raw, LeftBrace, std::set{TokenKindType::T_LEFT_BRACE});
   syntax_assert_child_token(raw, RightBrace, std::set{TokenKindType::T_RIGHT_BRACE});
   if (const RefCountPtr<RawSyntax> &exprChild = raw->getChild(Cursor::Expr)) {
      assert(exprChild->kindOf(SyntaxKind::Expr));
   }
#endif
}

TokenSyntax BraceDecoratedExprClauseSyntax::getLeftBrace()
{
   return TokenSyntax {m_root, m_data->getChild(Cursor::LeftBrace).get()};
}

ExprSyntax BraceDecoratedExprClauseSyntax::getExpr()
{
   return ExprSyntax {m_root, m_data->getChild(Cursor::Expr).get()};
}

TokenSyntax BraceDecoratedExprClauseSyntax::getRightBrace()
{
   return TokenSyntax {m_root, m_data->getChild(Cursor::RightBrace).get()};
}

BraceDecoratedExprClauseSyntax BraceDecoratedExprClauseSyntax::withLeftBrace(std::optional<TokenSyntax> leftBrace)
{
   RefCountPtr<RawSyntax> leftBraceRaw;
   if (leftBrace.has_value()) {
      leftBraceRaw = leftBrace->getRaw();
   } else {
      leftBraceRaw = RawSyntax::missing(TokenKindType::T_LEFT_BRACE,
                                        OwnedString::makeUnowned(get_token_text(TokenKindType::T_LEFT_BRACE)));
   }
   return m_data->replaceChild<BraceDecoratedExprClauseSyntax>(leftBraceRaw, Cursor::LeftBrace);
}

BraceDecoratedExprClauseSyntax BraceDecoratedExprClauseSyntax::withExpr(std::optional<ExprSyntax> expr)
{
   RefCountPtr<RawSyntax> exprRaw;
   if (expr.has_value()) {
      exprRaw = expr->getRaw();
   } else {
      exprRaw = RawSyntax::missing(SyntaxKind::UnknownExpr);
   }
   return m_data->replaceChild<BraceDecoratedExprClauseSyntax>(exprRaw, Cursor::Expr);
}

BraceDecoratedExprClauseSyntax BraceDecoratedExprClauseSyntax::withRightBrace(std::optional<TokenSyntax> rightBrace)
{
   RefCountPtr<RawSyntax> rightBraceRaw;
   if (rightBrace.has_value()) {
      rightBraceRaw = rightBrace->getRaw();
   } else {
      rightBraceRaw = RawSyntax::missing(TokenKindType::T_RIGHT_BRACE,
                                         OwnedString::makeUnowned(get_token_text(TokenKindType::T_RIGHT_BRACE)));
   }
   return m_data->replaceChild<BraceDecoratedExprClauseSyntax>(rightBraceRaw, Cursor::RightBrace);
}

///
/// BraceDecoratedVariableExprSyntax
///
void BraceDecoratedVariableExprSyntax::validate()
{
#ifdef POLAR_DEBUG_BUILD
   RefCountPtr<RawSyntax> raw = getRaw();
   if (isMissing()) {
      return;
   }
   assert(raw->getLayout().size() == BraceDecoratedVariableExprSyntax::CHILDREN_COUNT);
   syntax_assert_child_token(raw, DollarSign, std::set{TokenKindType::T_DOLLAR_SIGN});
   if (const RefCountPtr<RawSyntax> &exprChild = raw->getChild(Cursor::DecoratedExpr)) {
      assert(exprChild->kindOf(SyntaxKind::BraceDecoratedExprClause));
   }
#endif
}

TokenSyntax BraceDecoratedVariableExprSyntax::getDollarSign()
{
   return TokenSyntax {m_root, m_data->getChild(Cursor::DollarSign).get()};
}

BraceDecoratedExprClauseSyntax BraceDecoratedVariableExprSyntax::getDecoratedExpr()
{
   return BraceDecoratedExprClauseSyntax{m_root, m_data->getChild(Cursor::DecoratedExpr).get()};
}

BraceDecoratedVariableExprSyntax
BraceDecoratedVariableExprSyntax::withDollarSign(std::optional<TokenSyntax> dollarSign)
{
   RefCountPtr<RawSyntax> dollarSignRaw;
   if (dollarSign.has_value()) {
      dollarSignRaw = dollarSign->getRaw();
   } else {
      dollarSignRaw = RawSyntax::missing(TokenKindType::T_DOLLAR_SIGN,
                                         OwnedString::makeUnowned(get_token_text(TokenKindType::T_DOLLAR_SIGN)));
   }
   return m_data->replaceChild<BraceDecoratedVariableExprSyntax>(dollarSignRaw, Cursor::DollarSign);
}

BraceDecoratedVariableExprSyntax
BraceDecoratedVariableExprSyntax::withDecoratedExpr(std::optional<BraceDecoratedExprClauseSyntax> decoratedExpr)
{
   RefCountPtr<RawSyntax> decoratedExprRaw;
   if (decoratedExpr.has_value()) {
      decoratedExprRaw = decoratedExpr->getRaw();
   } else {
      decoratedExprRaw = RawSyntax::missing(SyntaxKind::BraceDecoratedExprClause);
   }
   return m_data->replaceChild<BraceDecoratedVariableExprSyntax>(decoratedExprRaw, Cursor::DecoratedExpr);
}

///
/// ArrayKeyValuePairItemSyntax
///
#ifdef POLAR_DEBUG_BUILD
const NodeChoicesType ArrayKeyValuePairItemSyntax::CHILD_NODE_CHOICES
{
   {
      ArrayKeyValuePairItemSyntax::Value, {
         SyntaxKind::Expr, SyntaxKind::VariableExpr
      }
   }
};
#endif

void ArrayKeyValuePairItemSyntax::validate()
{
#ifdef POLAR_DEBUG_BUILD
   RefCountPtr<RawSyntax> raw = getRaw();
   if (isMissing()) {
      return;
   }
   assert(raw->getLayout().size() == ArrayKeyValuePairItemSyntax::CHILDREN_COUNT);
   syntax_assert_child_token(raw, KeyExpr, std::set{TokenKindType::T_DOUBLE_ARROW});
   syntax_assert_child_token(raw, DoubleArrowToken, std::set{TokenKindType::T_DOUBLE_ARROW});
   syntax_assert_child_token(raw, ReferenceToken, std::set{TokenKindType::T_AMPERSAND});
   syntax_assert_child_kind(raw, Value, CHILD_NODE_CHOICES.at(Cursor::Value));
#endif
}

std::optional<ExprSyntax> ArrayKeyValuePairItemSyntax::getKeyExpr()
{
   RefCountPtr<SyntaxData> keyExprData = m_data->getChild(Cursor::KeyExpr);
   if (!keyExprData) {
      return std::nullopt;
   }
   return ExprSyntax {m_root, keyExprData.get()};
}

std::optional<TokenSyntax> ArrayKeyValuePairItemSyntax::getDoubleArrowToken()
{
   RefCountPtr<SyntaxData> doubleArrowTokenData = m_data->getChild(Cursor::DoubleArrowToken);
   if (!doubleArrowTokenData) {
      return std::nullopt;
   }
   return TokenSyntax {m_root, doubleArrowTokenData.get()};
}

std::optional<TokenSyntax> ArrayKeyValuePairItemSyntax::getReferenceToken()
{
   RefCountPtr<SyntaxData> referenceTokenData;
   if (!referenceTokenData) {
      return std::nullopt;
   }
   return TokenSyntax {m_root, referenceTokenData.get()};
}

ExprSyntax ArrayKeyValuePairItemSyntax::getValue()
{
   return ExprSyntax {m_root, m_data->getChild(Cursor::Value).get()};
}

ArrayKeyValuePairItemSyntax ArrayKeyValuePairItemSyntax::withKeyExpr(std::optional<ExprSyntax> keyExpr)
{
   RefCountPtr<RawSyntax> keyExprRaw;
   if (keyExpr.has_value()) {
      keyExprRaw = keyExpr->getRaw();
   } else {
      keyExprRaw = nullptr;
   }
   return m_data->replaceChild<ArrayKeyValuePairItemSyntax>(keyExprRaw, Cursor::KeyExpr);
}

ArrayKeyValuePairItemSyntax ArrayKeyValuePairItemSyntax::withDoubleArrowToken(std::optional<TokenSyntax> doubleArrowToken)
{
   RefCountPtr<RawSyntax> doubleArrowTokenRaw;
   if (doubleArrowToken.has_value()) {
      doubleArrowTokenRaw = doubleArrowToken->getRaw();
   } else {
      doubleArrowTokenRaw = nullptr;
   }
   return m_data->replaceChild<ArrayKeyValuePairItemSyntax>(doubleArrowTokenRaw, Cursor::DoubleArrowToken);
}

ArrayKeyValuePairItemSyntax ArrayKeyValuePairItemSyntax::withReferenceToken(std::optional<TokenSyntax> referenceToken)
{
   RefCountPtr<RawSyntax> referenceTokenRaw;
   if (referenceToken.has_value()) {
      referenceTokenRaw = referenceToken->getRaw();
   } else {
      referenceTokenRaw = nullptr;
   }
   return m_data->replaceChild<ArrayKeyValuePairItemSyntax>(referenceTokenRaw, Cursor::ReferenceToken);
}

ArrayKeyValuePairItemSyntax ArrayKeyValuePairItemSyntax::withValue(std::optional<ExprSyntax> value)
{
   RefCountPtr<RawSyntax> valueRaw;
   if (value.has_value()) {
      valueRaw = value->getRaw();
   } else {
      valueRaw = RawSyntax::missing(SyntaxKind::Expr);
   }
   return m_data->replaceChild<ArrayKeyValuePairItemSyntax>(valueRaw, Cursor::Value);
}

///
/// ArrayUnpackPairItemSyntax
///
void ArrayUnpackPairItemSyntax::validate()
{
#ifdef POLAR_DEBUG_BUILD
   RefCountPtr<RawSyntax> raw = getRaw();
   if (isMissing()) {
      return;
   }
   assert(raw->getLayout().size() == ArrayUnpackPairItemSyntax::CHILDREN_COUNT);
   syntax_assert_child_token(raw, EllipsisToken, std::set{TokenKindType::T_ELLIPSIS});
   syntax_assert_child_kind(raw, UnpackExpr, std::set{SyntaxKind::Expr});
#endif
}

TokenSyntax ArrayUnpackPairItemSyntax::getEllipsisToken()
{
   return TokenSyntax {m_root, m_data->getChild(Cursor::EllipsisToken).get()};
}

ExprSyntax ArrayUnpackPairItemSyntax::getUnpackExpr()
{
   return ExprSyntax {m_root, m_data->getChild(Cursor::UnpackExpr).get()};
}

ArrayUnpackPairItemSyntax ArrayUnpackPairItemSyntax::withEllipsisToken(std::optional<TokenSyntax> ellipsisToken)
{
   RefCountPtr<RawSyntax> ellipsisTokenRaw;
   if (ellipsisToken.has_value()) {
      ellipsisTokenRaw = ellipsisToken->getRaw();
   } else {
      ellipsisTokenRaw = RawSyntax::missing(TokenKindType::T_ELLIPSIS,
                                            OwnedString::makeUnowned(get_token_text(TokenKindType::T_ELLIPSIS)));
   }
   return m_data->replaceChild<ArrayUnpackPairItemSyntax>(ellipsisTokenRaw, Cursor::EllipsisToken);
}

ArrayUnpackPairItemSyntax ArrayUnpackPairItemSyntax::withUnpackExpr(std::optional<ExprSyntax> unpackExpr)
{
   RefCountPtr<RawSyntax> unpackExprRaw;
   if (unpackExpr.has_value()) {
      unpackExprRaw = unpackExpr->getRaw();
   } else {
      unpackExprRaw = RawSyntax::missing(SyntaxKind::Expr);
   }
   return m_data->replaceChild<ArrayUnpackPairItemSyntax>(unpackExprRaw, Cursor::UnpackExpr);
}

///
/// ArrayPairItemSyntax
///
#ifdef POLAR_DEBUG_BUILD
const NodeChoicesType ArrayPairItemSyntax::CHILD_NODE_CHOICES
{
   {
      ArrayPairItemSyntax::Item, {
         SyntaxKind::ArrayKeyValuePairItem,
               SyntaxKind::ArrayUnpackPairItem
      }
   }
};
#endif

void ArrayPairItemSyntax::validate()
{
#ifdef POLAR_DEBUG_BUILD
   RefCountPtr<RawSyntax> raw = getRaw();
   if (isMissing()) {
      return;
   }
   assert(raw->getLayout().size() == ArrayPairItemSyntax::CHILDREN_COUNT);
   syntax_assert_child_kind(raw, Item, CHILD_NODE_CHOICES.at(Cursor::Item));
#endif
}

Syntax ArrayPairItemSyntax::getItem()
{
   return Syntax {m_root, m_data->getChild(Cursor::Item).get()};
}

std::optional<TokenSyntax> ArrayPairItemSyntax::getTrailingComma()
{
   RefCountPtr<SyntaxData> commaData = m_data->getChild(Cursor::TrailingComma);
   if (!commaData) {
      return std::nullopt;
   }
   return TokenSyntax {m_root, commaData.get()};
}

ArrayPairItemSyntax ArrayPairItemSyntax::withItem(std::optional<Syntax> item)
{
   RefCountPtr<RawSyntax> itemRaw;
   if (item.has_value()) {
      itemRaw = item->getRaw();
   } else {
      itemRaw = RawSyntax::missing(SyntaxKind::Unknown);
   }
   return m_data->replaceChild<ArrayPairItemSyntax>(itemRaw, Cursor::Item);
}

ArrayPairItemSyntax ArrayPairItemSyntax::withTrailingComma(std::optional<TokenSyntax> trailingComma)
{
   RefCountPtr<RawSyntax> trailingCommaRaw;
   if (trailingComma.has_value()) {
      trailingCommaRaw = trailingComma->getRaw();
   } else {
      trailingCommaRaw = nullptr;
   }
   return m_data->replaceChild<ArrayPairItemSyntax>(trailingCommaRaw, Cursor::TrailingComma);
}

///
/// ArrayKeyValuePairItemSyntax
///
void ListRecursivePairItemSyntax::validate()
{
#ifdef POLAR_DEBUG_BUILD
   RefCountPtr<RawSyntax> raw = getRaw();
   if (isMissing()) {
      return;
   }
   assert(raw->getLayout().size() == ListRecursivePairItemSyntax::CHILDREN_COUNT);
   syntax_assert_child_kind(raw, KeyExpr, std::set{SyntaxKind::Expr});
   syntax_assert_child_kind(raw, ListPairItemList, std::set{SyntaxKind::ListPairItemList});
   syntax_assert_child_token(raw, DoubleArrowToken, std::set{TokenKindType::T_DOUBLE_ARROW});
   syntax_assert_child_token(raw, ListToken, std::set{TokenKindType::T_LIST});
   syntax_assert_child_token(raw, LeftParen, std::set{TokenKindType::T_LEFT_PAREN});
   syntax_assert_child_token(raw, RightParen, std::set{TokenKindType::T_RIGHT_PAREN});
#endif
}

std::optional<ExprSyntax> ListRecursivePairItemSyntax::getKeyExpr()
{
   RefCountPtr<SyntaxData> keyExprData = m_data->getChild(Cursor::KeyExpr);
   if (!keyExprData) {
      return std::nullopt;
   }
   return ExprSyntax {m_root, keyExprData.get()};
}

std::optional<TokenSyntax> ListRecursivePairItemSyntax::getDoubleArrowToken()
{
   RefCountPtr<SyntaxData> arrowData = m_data->getChild(Cursor::DoubleArrowToken);
   if (!arrowData) {
      return std::nullopt;
   }
   return TokenSyntax {m_root, arrowData.get()};
}

TokenSyntax ListRecursivePairItemSyntax::getListToken()
{
   return TokenSyntax {m_root, m_data->getChild(Cursor::ListToken).get()};
}

TokenSyntax ListRecursivePairItemSyntax::getLeftParen()
{
   return TokenSyntax {m_root, m_data->getChild(Cursor::LeftParen).get()};
}

ListPairItemListSyntax ListRecursivePairItemSyntax::getListPairItemList()
{
   return ListPairItemListSyntax {m_root, m_data->getChild(Cursor::ListPairItemList).get()};
}

TokenSyntax ListRecursivePairItemSyntax::getRightParen()
{
   return TokenSyntax {m_root, m_data->getChild(Cursor::RightParen).get()};
}

ListRecursivePairItemSyntax
ListRecursivePairItemSyntax::withKeyExpr(std::optional<ExprSyntax> keyExpr)
{
   RefCountPtr<RawSyntax> keyExprRaw;
   if (keyExpr.has_value()) {
      keyExprRaw = keyExpr->getRaw();
   } else {
      keyExprRaw = nullptr;
   }
   return m_data->replaceChild<ListRecursivePairItemSyntax>(keyExprRaw, Cursor::KeyExpr);
}

ListRecursivePairItemSyntax
ListRecursivePairItemSyntax::withDoubleArrowToken(std::optional<TokenSyntax> doubleArrowToken)
{
   RefCountPtr<RawSyntax> doubleArrowTokenRaw;
   if (doubleArrowToken.has_value()) {
      doubleArrowTokenRaw = doubleArrowToken->getRaw();
   } else {
      doubleArrowTokenRaw = nullptr;
   }
   return m_data->replaceChild<ListRecursivePairItemSyntax>(doubleArrowTokenRaw, Cursor::DoubleArrowToken);
}

ListRecursivePairItemSyntax
ListRecursivePairItemSyntax::withListToken(std::optional<TokenSyntax> listToken)
{
   RefCountPtr<RawSyntax> listTokenRaw;
   if (listToken.has_value()) {
      listTokenRaw = listToken->getRaw();
   } else {
      listTokenRaw = RawSyntax::missing(TokenKindType::T_LIST,
                                        OwnedString::makeUnowned(get_token_text(TokenKindType::T_LIST)));
   }
   return m_data->replaceChild<ListRecursivePairItemSyntax>(listTokenRaw, Cursor::ListPairItemList);
}

ListRecursivePairItemSyntax
ListRecursivePairItemSyntax::withLeftParen(std::optional<TokenSyntax> leftParen)
{
   RefCountPtr<RawSyntax> leftParenRaw;
   if (leftParen.has_value()) {
      leftParenRaw = leftParen->getRaw();
   } else {
      leftParenRaw = RawSyntax::missing(TokenKindType::T_LEFT_PAREN,
                                        OwnedString::makeUnowned(get_token_text(TokenKindType::T_LEFT_PAREN)));
   }
   return m_data->replaceChild<ListRecursivePairItemSyntax>(leftParenRaw, Cursor::LeftParen);
}

ListRecursivePairItemSyntax
ListRecursivePairItemSyntax::withListPairItemList(std::optional<ListPairItemListSyntax> pairItemList)
{
   RefCountPtr<RawSyntax> pairItemListRaw;
   if (pairItemList.has_value()) {
      pairItemListRaw = pairItemList->getRaw();
   } else {
      pairItemListRaw = RawSyntax::missing(SyntaxKind::ListPairItemList);
   }
   return m_data->replaceChild<ListRecursivePairItemSyntax>(pairItemListRaw, Cursor::ListPairItemList);
}

ListRecursivePairItemSyntax ListRecursivePairItemSyntax::withRightParen(std::optional<TokenSyntax> rightParen)
{
   RefCountPtr<RawSyntax> rightParenRaw;
   if (rightParen.has_value()) {
      rightParenRaw = rightParen->getRaw();
   } else {
      rightParenRaw = RawSyntax::missing(TokenKindType::T_RIGHT_PAREN,
                                         OwnedString::makeUnowned(get_token_text(TokenKindType::T_RIGHT_PAREN)));
   }
   return m_data->replaceChild<ListRecursivePairItemSyntax>(rightParenRaw, Cursor::RightParen);
}

///
/// ListPairItemSyntax
///
#ifdef POLAR_DEBUG_BUILD
const NodeChoicesType ListPairItemSyntax::CHILD_NODE_CHOICES
{
   {
      ListPairItemSyntax::Item, {
         SyntaxKind::ArrayPairItem, SyntaxKind::ListRecursivePairItem
      }
   }
};
#endif

void ListPairItemSyntax::validate()
{
#ifdef POLAR_DEBUG_BUILD
   RefCountPtr<RawSyntax> raw = getRaw();
   if (isMissing()) {
      return;
   }
   assert(raw->getLayout().size() == ListPairItemSyntax::CHILDREN_COUNT);
   syntax_assert_child_kind(raw, Item, CHILD_NODE_CHOICES.at(Cursor::Item));
   syntax_assert_child_token(raw, TrailingComma, std::set{TokenKindType::T_COMMA});
#endif
}

Syntax ListPairItemSyntax::getItem()
{
   return Syntax {m_root, m_data->getChild(Cursor::Item).get()};
}

std::optional<TokenSyntax> ListPairItemSyntax::getTrailingComma()
{
   RefCountPtr<SyntaxData> commaData = m_data->getChild(Cursor::TrailingComma);
   if (!commaData) {
      return std::nullopt;
   }
   return TokenSyntax {m_root, commaData.get()};
}

ListPairItemSyntax ListPairItemSyntax::withItem(std::optional<Syntax> item)
{
   RefCountPtr<RawSyntax> itemRaw;
   if (item.has_value()) {
      itemRaw = item->getRaw();
   } else {
      itemRaw = RawSyntax::missing(SyntaxKind::ArrayPairItem);
   }
   return m_data->replaceChild<ListPairItemSyntax>(itemRaw, Cursor::Item);
}

ListPairItemSyntax ListPairItemSyntax::withTrailingComma(std::optional<TokenSyntax> trailingComma)
{
   RefCountPtr<RawSyntax> trailingCommaRaw;
   if (trailingComma.has_value()) {
      trailingCommaRaw = trailingComma->getRaw();
   } else {
      trailingCommaRaw = nullptr;
   }
   return m_data->replaceChild<ListPairItemSyntax>(trailingCommaRaw, Cursor::TrailingComma);
}

///
/// SimpleVariableExprSyntax
///
#ifdef POLAR_DEBUG_BUILD
const NodeChoicesType SimpleVariableExprSyntax::CHILD_NODE_CHOICES
{
   {
      SimpleVariableExprSyntax::Variable, {
         SyntaxKind::BraceDecoratedVariableExpr,
               SyntaxKind::SimpleVariableExpr
      }
   }
};
#endif

void SimpleVariableExprSyntax::validate()
{
#ifdef POLAR_DEBUG_BUILD
   RefCountPtr<RawSyntax> raw = getRaw();
   if (isMissing()) {
      return;
   }
   assert(raw->getLayout().size() == SimpleVariableExprSyntax::CHILDREN_COUNT);
   syntax_assert_child_token(raw, DollarSign, std::set{TokenKindType::T_DOLLAR_SIGN});
   if (const RefCountPtr<RawSyntax> &variableChild = raw->getChild(Cursor::Variable)) {
      if (variableChild->isToken()) {
         syntax_assert_child_token(raw, Variable, std::set{TokenKindType::T_VARIABLE});
      } else {
         syntax_assert_child_kind(raw, Variable, CHILD_NODE_CHOICES.at(Cursor::Variable));
      }
      if (variableChild->kindOf(SyntaxKind::SimpleVariableExpr)) {
         assert(raw->getChild(Cursor::DollarSign));
      }
   }
#endif
}

std::optional<TokenSyntax> SimpleVariableExprSyntax::getDollarSign()
{
   RefCountPtr<SyntaxData> dollarSignData = m_data->getChild(Cursor::DollarSign);
   if (!dollarSignData) {
      return std::nullopt;
   }
   return TokenSyntax {m_root, dollarSignData.get()};
}

Syntax SimpleVariableExprSyntax::getVariable()
{
   return Syntax {m_root, m_data->getChild(Cursor::Variable).get()};
}

SimpleVariableExprSyntax SimpleVariableExprSyntax::withDollarSign(std::optional<TokenSyntax> dollarSign)
{
   RefCountPtr<RawSyntax> dollarSignRaw;
   if (dollarSign.has_value()) {
      dollarSignRaw = dollarSign->getRaw();
   } else {
      dollarSignRaw = nullptr;
   }
   return m_data->replaceChild<SimpleVariableExprSyntax>(dollarSignRaw, Cursor::DollarSign);
}

SimpleVariableExprSyntax SimpleVariableExprSyntax::withVariable(std::optional<Syntax> variable)
{
   RefCountPtr<RawSyntax> variableRaw;
   if (variable.has_value()) {
      variableRaw = variable->getRaw();
   } else {
      variableRaw = RawSyntax::missing(SyntaxKind::Unknown);
   }
   return m_data->replaceChild<SimpleVariableExprSyntax>(variableRaw, Cursor::Variable);
}

///
/// ArrayExprSyntax
///
void ArrayExprSyntax::validate()
{
#ifdef POLAR_DEBUG_BUILD
   RefCountPtr<RawSyntax> raw = getRaw();
   if (isMissing()) {
      return;
   }
   assert(raw->getLayout().size() == ArrayExprSyntax::CHILDREN_COUNT);
   syntax_assert_child_token(raw, ArrayToken, std::set{TokenKindType::T_ARRAY});
   syntax_assert_child_token(raw, LeftParen, std::set{TokenKindType::T_LEFT_PAREN});
   syntax_assert_child_token(raw, RightParen, std::set{TokenKindType::T_RIGHT_PAREN});
   syntax_assert_child_kind(raw, PairItemList, std::set{SyntaxKind::ArrayPairItemList});
#endif
}

TokenSyntax ArrayExprSyntax::getArrayToken()
{
   return TokenSyntax {m_root, m_data->getChild(Cursor::ArrayToken).get()};
}

TokenSyntax ArrayExprSyntax::getLeftParen()
{
   return TokenSyntax {m_root, m_data->getChild(Cursor::LeftParen).get()};
}

ArrayPairItemListSyntax ArrayExprSyntax::getPairItemList()
{
   return ArrayPairItemListSyntax {m_root, m_data->getChild(Cursor::PairItemList).get()};
}

TokenSyntax ArrayExprSyntax::getRightParen()
{
   return TokenSyntax {m_root, m_data->getChild(Cursor::RightParen).get()};
}

ArrayExprSyntax ArrayExprSyntax::withArrayToken(std::optional<TokenSyntax> arrayToken)
{
   RefCountPtr<RawSyntax> arrayTokenRaw;
   if (arrayToken.has_value()) {
      arrayTokenRaw = arrayToken->getRaw();
   } else {
      arrayTokenRaw = RawSyntax::missing(TokenKindType::T_ARRAY,
                                         OwnedString::makeUnowned(get_token_text(TokenKindType::T_ARRAY)));
   }
   return m_data->replaceChild<ArrayExprSyntax>(arrayTokenRaw, Cursor::ArrayToken);
}

ArrayExprSyntax ArrayExprSyntax::withLeftParen(std::optional<TokenSyntax> leftParen)
{
   RefCountPtr<RawSyntax> leftParanRaw;
   if (leftParen.has_value()) {
      leftParanRaw = leftParen->getRaw();
   } else {
      leftParanRaw = RawSyntax::missing(TokenKindType::T_LEFT_PAREN,
                                        OwnedString::makeUnowned(get_token_text(TokenKindType::T_LEFT_PAREN)));
   }
   return m_data->replaceChild<ArrayExprSyntax>(leftParanRaw, Cursor::LeftParen);
}

ArrayExprSyntax ArrayExprSyntax::withPairItemList(std::optional<ArrayPairItemListSyntax> pairItemList)
{
   RefCountPtr<RawSyntax> pairItemListRaw;
   if (pairItemList.has_value()) {
      pairItemListRaw = pairItemList->getRaw();
   } else {
      pairItemListRaw = RawSyntax::missing(SyntaxKind::ArrayPairItemList);
   }
   return m_data->replaceChild<ArrayExprSyntax>(pairItemListRaw, Cursor::PairItemList);
}

ArrayExprSyntax ArrayExprSyntax::withRightParen(std::optional<TokenSyntax> rightParen)
{
   RefCountPtr<RawSyntax> rightParenRaw;
   if (rightParen.has_value()) {
      rightParenRaw = rightParen->getRaw();
   } else {
      rightParenRaw = RawSyntax::missing(TokenKindType::T_RIGHT_PAREN,
                                        OwnedString::makeUnowned(get_token_text(TokenKindType::T_RIGHT_PAREN)));
   }
   return m_data->replaceChild<ArrayExprSyntax>(rightParenRaw, Cursor::RightParen);
}

///
/// SimplifiedArrayExprSyntax
///
void SimplifiedArrayExprSyntax::validate()
{
#ifdef POLAR_DEBUG_BUILD
   RefCountPtr<RawSyntax> raw = getRaw();
   if (isMissing()) {
      return;
   }
   assert(raw->getLayout().size() == SimplifiedArrayExprSyntax::CHILDREN_COUNT);
   syntax_assert_child_token(raw, LeftSquareBracket, std::set{TokenKindType::T_LEFT_SQUARE_BRACKET});
   syntax_assert_child_token(raw, RightSquareBracket, std::set{TokenKindType::T_RIGHT_SQUARE_BRACKET});
   syntax_assert_child_kind(raw, PairItemList, std::set{SyntaxKind::ArrayPairItemList});
#endif
}

TokenSyntax SimplifiedArrayExprSyntax::getLeftSquareBracket()
{
   return TokenSyntax {m_root, m_data->getChild(Cursor::LeftSquareBracket).get()};
}

ArrayPairItemListSyntax SimplifiedArrayExprSyntax::getPairItemList()
{
   return ArrayPairItemListSyntax {m_root, m_data->getChild(Cursor::PairItemList).get()};
}

TokenSyntax SimplifiedArrayExprSyntax::getRightSquareBracket()
{
   return TokenSyntax {m_root, m_data->getChild(Cursor::RightSquareBracket).get()};
}

SimplifiedArrayExprSyntax
SimplifiedArrayExprSyntax::withLeftSquareBracket(std::optional<TokenSyntax> leftSquareBracket)
{
   RefCountPtr<RawSyntax> leftSquareBracketRaw;
   if (leftSquareBracket.has_value()) {
      leftSquareBracketRaw = leftSquareBracket->getRaw();
   } else {
      leftSquareBracketRaw = RawSyntax::missing(TokenKindType::T_LEFT_SQUARE_BRACKET,
                                                OwnedString::makeUnowned(get_token_text(TokenKindType::T_LEFT_SQUARE_BRACKET)));
   }
   return m_data->replaceChild<SimplifiedArrayExprSyntax>(leftSquareBracketRaw, Cursor::LeftSquareBracket);
}

SimplifiedArrayExprSyntax
SimplifiedArrayExprSyntax::withPairItemList(std::optional<ArrayPairItemListSyntax> pairItemList)
{
   RefCountPtr<RawSyntax> pairItemListRaw;
   if (pairItemList.has_value()) {
      pairItemListRaw = pairItemList->getRaw();
   } else {
      pairItemListRaw = RawSyntax::missing(SyntaxKind::ArrayPairItemList);
   }
   return m_data->replaceChild<SimplifiedArrayExprSyntax>(pairItemListRaw, Cursor::PairItemList);
}

SimplifiedArrayExprSyntax
SimplifiedArrayExprSyntax::withRightSquareBracket(std::optional<TokenSyntax> rightSquareBracket)
{
   RefCountPtr<RawSyntax> rightSquareBracketRaw;
   if (rightSquareBracket.has_value()) {
      rightSquareBracketRaw = rightSquareBracket->getRaw();
   } else {
      rightSquareBracketRaw = RawSyntax::missing(TokenKindType::T_RIGHT_SQUARE_BRACKET,
                                                OwnedString::makeUnowned(get_token_text(TokenKindType::T_RIGHT_SQUARE_BRACKET)));
   }
   return m_data->replaceChild<SimplifiedArrayExprSyntax>(rightSquareBracketRaw, Cursor::RightSquareBracket);
}

///
/// ClassRefParentExprSyntax
///
void ClassRefParentExprSyntax::validate()
{
#ifdef POLAR_DEBUG_BUILD
   RefCountPtr<RawSyntax> raw = m_data->getRaw();
   if (isMissing()) {
      return;
   }
   assert(raw->getLayout().size() == ClassRefParentExprSyntax::CHILDREN_COUNT);
#endif
}

TokenSyntax ClassRefParentExprSyntax::getParentKeyword()
{
   return TokenSyntax{m_root, m_data->getChild(Cursor::ParentKeyword).get()};
}

ClassRefParentExprSyntax ClassRefParentExprSyntax::withParentKeyword(std::optional<TokenSyntax> parentKeyword)
{
   RefCountPtr<RawSyntax> rawParentKeyword;
   if (parentKeyword.has_value()) {
      rawParentKeyword = parentKeyword->getRaw();
   } else {
      rawParentKeyword = RawSyntax::missing(TokenKindType::T_CLASS_REF_PARENT,
                                            OwnedString::makeUnowned(get_token_text(TokenKindType::T_CLASS_REF_PARENT)));
   }
   return m_data->replaceChild<ClassRefParentExprSyntax>(rawParentKeyword, Cursor::ParentKeyword);
}

void ClassRefSelfExprSyntax::validate()
{
#ifdef POLAR_DEBUG_BUILD
   RefCountPtr<RawSyntax> raw = m_data->getRaw();
   if (isMissing()) {
      return;
   }
   assert(raw->getLayout().size() == ClassRefSelfExprSyntax::CHILDREN_COUNT);
#endif
}

TokenSyntax ClassRefSelfExprSyntax::getSelfKeyword()
{
   return TokenSyntax{m_root, m_data->getChild(Cursor::SelfKeyword).get()};
}

ClassRefSelfExprSyntax ClassRefSelfExprSyntax::withSelfKeyword(std::optional<TokenSyntax> selfKeyword)
{
   RefCountPtr<RawSyntax> rawSelfKeyword;
   if (selfKeyword.has_value()) {
      rawSelfKeyword = selfKeyword->getRaw();
   } else {
      rawSelfKeyword = RawSyntax::missing(TokenKindType::T_CLASS_REF_SELF,
                                          OwnedString::makeUnowned(get_token_text(TokenKindType::T_CLASS_REF_SELF)));
   }
   return m_data->replaceChild<ClassRefSelfExprSyntax>(rawSelfKeyword, Cursor::SelfKeyword);
}

void ClassRefStaticExprSyntax::validate()
{
#ifdef POLAR_DEBUG_BUILD
   RefCountPtr<RawSyntax> raw = m_data->getRaw();
   if (isMissing()) {
      return;
   }
   assert(raw->getLayout().size() == ClassRefStaticExprSyntax::CHILDREN_COUNT);
#endif
}

TokenSyntax ClassRefStaticExprSyntax::getStaticKeyword()
{
   return TokenSyntax{m_root, m_data->getChild(Cursor::StaticKeyword).get()};
}

ClassRefStaticExprSyntax ClassRefStaticExprSyntax::withStaticKeyword(std::optional<TokenSyntax> staticKeyword)
{
   RefCountPtr<RawSyntax> rawStaticKeyword;
   if (staticKeyword.has_value()) {
      rawStaticKeyword = staticKeyword->getRaw();
   } else {
      rawStaticKeyword = RawSyntax::missing(TokenKindType::T_CLASS_REF_STATIC,
                                            OwnedString::makeUnowned(get_token_text(TokenKindType::T_CLASS_REF_STATIC)));
   }
   return m_data->replaceChild<ClassRefStaticExprSyntax>(rawStaticKeyword, Cursor::StaticKeyword);
}

///
/// IntegerLiteralExprSyntax
///
void IntegerLiteralExprSyntax::validate()
{
#ifdef POLAR_DEBUG_BUILD
   RefCountPtr<RawSyntax> raw = m_data->getRaw();
   if (isMissing()) {
      return;
   }
   assert(raw->getLayout().size() == IntegerLiteralExprSyntax::CHILDREN_COUNT);
#endif
}

TokenSyntax IntegerLiteralExprSyntax::getDigits()
{
   return TokenSyntax{m_root, m_data->getChild(Cursor::Digits).get()};
}

IntegerLiteralExprSyntax IntegerLiteralExprSyntax::withDigits(std::optional<TokenSyntax> digits)
{
   RefCountPtr<RawSyntax> rawDigits;
   if (digits.has_value()) {
      rawDigits = digits->getRaw();
   } else {
      rawDigits = RawSyntax::missing(TokenKindType::T_LNUMBER,
                                     OwnedString::makeUnowned(get_token_text(TokenKindType::T_LNUMBER)));
   }
   return m_data->replaceChild<IntegerLiteralExprSyntax>(rawDigits, Cursor::Digits);
}

///
/// FloatLiteralExprSyntax
///
void FloatLiteralExprSyntax::validate()
{
#ifdef POLAR_DEBUG_BUILD
   RefCountPtr<RawSyntax> raw = m_data->getRaw();
   if (isMissing()) {
      return;
   }
   assert(raw->getLayout().size() == FloatLiteralExprSyntax::CHILDREN_COUNT);
#endif
}

TokenSyntax FloatLiteralExprSyntax::getFloatDigits()
{
   return TokenSyntax{m_root, m_data->getChild(Cursor::FloatDigits).get()};
}

FloatLiteralExprSyntax FloatLiteralExprSyntax::withFloatDigits(std::optional<TokenSyntax> digits)
{
   RefCountPtr<RawSyntax> rawDigits;
   if (digits.has_value()) {
      rawDigits = digits->getRaw();
   } else {
      rawDigits = RawSyntax::missing(TokenKindType::T_DNUMBER,
                                     OwnedString::makeUnowned(get_token_text(TokenKindType::T_DNUMBER)));
   }
   return m_data->replaceChild<FloatLiteralExprSyntax>(rawDigits, Cursor::FloatDigits);
}

///
/// StringLiteralExprSyntax
///

#ifdef POLAR_DEBUG_BUILD
const TokenChoicesType StringLiteralExprSyntax::CHILD_TOKEN_CHOICES
{
   {
      StringLiteralExprSyntax::LeftQuote, {
         TokenKindType::T_SINGLE_QUOTE, TokenKindType::T_DOUBLE_QUOTE
      }
   },
   {
      StringLiteralExprSyntax::RightQuote, {
         TokenKindType::T_SINGLE_QUOTE, TokenKindType::T_DOUBLE_QUOTE
      }
   }
};
#endif

void StringLiteralExprSyntax::validate()
{
#ifdef POLAR_DEBUG_BUILD
   RefCountPtr<RawSyntax> raw = m_data->getRaw();
   if (isMissing()) {
      return;
   }
   assert(raw->getLayout().size() == StringLiteralExprSyntax::CHILDREN_COUNT);
   syntax_assert_child_token(raw, LeftQuote, CHILD_TOKEN_CHOICES.at(Cursor::LeftQuote));
   syntax_assert_child_token(raw, RightQuote, CHILD_TOKEN_CHOICES.at(Cursor::RightQuote));
   assert(raw->getChild(Cursor::LeftQuote)->getTokenKind() == raw->getChild(Cursor::RightQuote)->getTokenKind());
#endif
}

TokenSyntax StringLiteralExprSyntax::getLeftQuote()
{
   return TokenSyntax {m_root, m_data->getChild(Cursor::LeftQuote).get()};
}

TokenSyntax StringLiteralExprSyntax::getText()
{
   return TokenSyntax {m_root, m_data->getChild(Cursor::Text).get()};
}

TokenSyntax StringLiteralExprSyntax::getRightQuote()
{
   return TokenSyntax {m_root, m_data->getChild(Cursor::RightQuote).get()};
}

StringLiteralExprSyntax StringLiteralExprSyntax::withLeftQuote(std::optional<TokenSyntax> leftQuote)
{
   RefCountPtr<RawSyntax> leftQuoteRaw;
   if (leftQuote.has_value()) {
      leftQuoteRaw = leftQuote->getRaw();
   } else {
      leftQuoteRaw = RawSyntax::missing(TokenKindType::T_DOUBLE_QUOTE,
                                        OwnedString::makeUnowned(get_token_text(TokenKindType::T_DOUBLE_QUOTE)));
   }
   return m_data->replaceChild<StringLiteralExprSyntax>(leftQuoteRaw, Cursor::LeftQuote);
}

StringLiteralExprSyntax StringLiteralExprSyntax::withText(std::optional<TokenSyntax> text)
{
   RefCountPtr<RawSyntax> textRaw;
   if (text.has_value()) {
      textRaw = text->getRaw();
   } else {
      textRaw = RawSyntax::missing(TokenKindType::T_CONSTANT_ENCAPSED_STRING,
                                   OwnedString::makeUnowned(get_token_text(TokenKindType::T_CONSTANT_ENCAPSED_STRING)));
   }
   return m_data->replaceChild<StringLiteralExprSyntax>(textRaw, Cursor::Text);
}

StringLiteralExprSyntax StringLiteralExprSyntax::withRightQuote(std::optional<TokenSyntax> rightQuote)
{
   RefCountPtr<RawSyntax> rightQuoteRaw;
   if (rightQuote.has_value()) {
      rightQuoteRaw = rightQuote->getRaw();
   } else {
      RefCountPtr<RawSyntax> leftQuote = getRaw()->getChild(Cursor::LeftQuote);
      if (leftQuote) {
         TokenKindType leftQuoteKind = leftQuote->getTokenKind();
         rightQuoteRaw = RawSyntax::missing(leftQuoteKind,
                                            OwnedString::makeUnowned(get_token_text(leftQuoteKind)));
      } else {
         rightQuoteRaw = RawSyntax::missing(TokenKindType::T_DOUBLE_QUOTE,
                                            OwnedString::makeUnowned(get_token_text(TokenKindType::T_DOUBLE_QUOTE)));
      }
   }
   return m_data->replaceChild<StringLiteralExprSyntax>(rightQuoteRaw, Cursor::LeftQuote);
}

///
/// BooleanLiteralExprSyntax
///
#ifdef POLAR_DEBUG_BUILD
const TokenChoicesType BooleanLiteralExprSyntax::CHILD_TOKEN_CHOICES
{
   {
      BooleanLiteralExprSyntax::Boolean, {
         TokenKindType::T_FALSE,
               TokenKindType::T_TRUE
      }
   }
};
#endif

void BooleanLiteralExprSyntax::validate()
{
#ifdef POLAR_DEBUG_BUILD
   RefCountPtr<RawSyntax> raw = m_data->getRaw();
   if (isMissing()) {
      return;
   }
   ///
   /// check Boolean token choice
   ///
   syntax_assert_child_token(raw, Boolean, CHILD_TOKEN_CHOICES.at(Boolean));
   assert(raw->getLayout().size() == BooleanLiteralExprSyntax::CHILDREN_COUNT);
#endif
}

TokenSyntax BooleanLiteralExprSyntax::getBooleanValue()
{
   return TokenSyntax{m_root, m_data->getChild(Cursor::Boolean).get()};
}

BooleanLiteralExprSyntax BooleanLiteralExprSyntax::withBooleanValue(std::optional<TokenSyntax> booleanValue)
{
   RefCountPtr<RawSyntax> rawBooleanValue;
   if (booleanValue.has_value()) {
      rawBooleanValue = booleanValue->getRaw();
   } else {
      rawBooleanValue = RawSyntax::missing(TokenKindType::T_TRUE,
                                           OwnedString::makeUnowned(get_token_text(TokenKindType::T_TRUE)));
   }
   return m_data->replaceChild<BooleanLiteralExprSyntax>(rawBooleanValue, Cursor::Boolean);
}

void TernaryExprSyntax::validate()
{
#ifdef POLAR_DEBUG_BUILD
   RefCountPtr<RawSyntax> raw = m_data->getRaw();
   if (isMissing()) {
      return;
   }
   assert(raw->getLayout().size() == TernaryExprSyntax::CHILDREN_COUNT);
#endif
}

ExprSyntax TernaryExprSyntax::getConditionExpr()
{
   return ExprSyntax{m_root, m_data->getChild(Cursor::ConditionExpr).get()};
}

TokenSyntax TernaryExprSyntax::getQuestionMark()
{
   return TokenSyntax{m_root, m_data->getChild(Cursor::QuestionMark).get()};
}

ExprSyntax TernaryExprSyntax::getFirstChoice()
{
   return ExprSyntax{m_root, m_data->getChild(Cursor::FirstChoice).get()};
}

TokenSyntax TernaryExprSyntax::getColonMark()
{
   return TokenSyntax{m_root, m_data->getChild(Cursor::ColonMark).get()};
}

ExprSyntax TernaryExprSyntax::getSecondChoice()
{
   return ExprSyntax{m_root, m_data->getChild(Cursor::SecondChoice).get()};
}

TernaryExprSyntax TernaryExprSyntax::withConditionExpr(std::optional<ExprSyntax> conditionExpr)
{
   RefCountPtr<RawSyntax> rawConditionExpr;
   if (conditionExpr.has_value()) {
      rawConditionExpr = conditionExpr->getRaw();
   } else {
      rawConditionExpr = RawSyntax::missing(SyntaxKind::Expr);
   }
   return m_data->replaceChild<TernaryExprSyntax>(rawConditionExpr, Cursor::ConditionExpr);
}

TernaryExprSyntax TernaryExprSyntax::withQuestionMark(std::optional<TokenSyntax> questionMark)
{
   RefCountPtr<RawSyntax> rawQuestionMark;
   if (questionMark.has_value()) {
      rawQuestionMark = questionMark->getRaw();
   } else {
      rawQuestionMark = RawSyntax::missing(TokenKindType::T_QUESTION_MARK,
                                           OwnedString::makeUnowned(get_token_text(TokenKindType::T_QUESTION_MARK)));
   }
   return m_data->replaceChild<TernaryExprSyntax>(rawQuestionMark, Cursor::QuestionMark);
}

TernaryExprSyntax TernaryExprSyntax::withFirstChoice(std::optional<ExprSyntax> firstChoice)
{
   RefCountPtr<RawSyntax> rawFirstChoice;
   if (firstChoice.has_value()) {
      rawFirstChoice = firstChoice->getRaw();
   } else {
      rawFirstChoice = RawSyntax::missing(SyntaxKind::Expr);
   }
   return m_data->replaceChild<TernaryExprSyntax>(rawFirstChoice, Cursor::FirstChoice);
}

TernaryExprSyntax TernaryExprSyntax::withColonMark(std::optional<TokenSyntax> colonMark)
{
   RefCountPtr<RawSyntax> rawColonMark;
   if (colonMark.has_value()) {
      rawColonMark = colonMark->getRaw();
   } else {
      rawColonMark = RawSyntax::missing(TokenKindType::T_COLON,
                                        OwnedString::makeUnowned(get_token_text(TokenKindType::T_COLON)));
   }
   return m_data->replaceChild<TernaryExprSyntax>(rawColonMark, Cursor::ColonMark);
}

TernaryExprSyntax TernaryExprSyntax::withSecondChoice(std::optional<ExprSyntax> secondChoice)
{
   RefCountPtr<RawSyntax> rawSecondChoice;
   if (secondChoice.has_value()) {
      rawSecondChoice = secondChoice->getRaw();
   } else {
      rawSecondChoice = RawSyntax::missing(SyntaxKind::Expr);
   }
   return m_data->replaceChild<TernaryExprSyntax>(rawSecondChoice, Cursor::SecondChoice);
}

void AssignmentExprSyntax::validate()
{
#ifdef POLAR_DEBUG_BUILD
   RefCountPtr<RawSyntax> raw = m_data->getRaw();
   if (isMissing()) {
      return;
   }
   assert(raw->getLayout().size() == AssignmentExprSyntax::CHILDREN_COUNT);
#endif
}

TokenSyntax AssignmentExprSyntax::getAssignToken()
{
   return TokenSyntax{m_root, m_data->getChild(Cursor::AssignToken).get()};
}

AssignmentExprSyntax AssignmentExprSyntax::withAssignToken(std::optional<TokenSyntax> assignToken)
{
   RefCountPtr<RawSyntax> rawAssignToken;
   if (assignToken.has_value()) {
      rawAssignToken = assignToken->getRaw();
   } else {
      rawAssignToken = RawSyntax::missing(TokenKindType::T_EQUAL,
                                          OwnedString::makeUnowned(get_token_text(TokenKindType::T_EQUAL)));
   }
   return m_data->replaceChild<AssignmentExprSyntax>(rawAssignToken, Cursor::AssignToken);
}

void SequenceExprSyntax::validate()
{
#ifdef POLAR_DEBUG_BUILD
   RefCountPtr<RawSyntax> raw = m_data->getRaw();
   if (isMissing()) {
      return;
   }
   assert(raw->getLayout().size() == SequenceExprSyntax::CHILDREN_COUNT);
#endif
}

ExprListSyntax SequenceExprSyntax::getElements()
{
   return ExprListSyntax{m_root, m_data->getChild(Cursor::Elements).get()};
}

SequenceExprSyntax SequenceExprSyntax::withElements(std::optional<ExprListSyntax> elements)
{
   RefCountPtr<RawSyntax> rawElements;
   if (elements.has_value()) {
      rawElements = elements->getRaw();
   } else {
      rawElements = RawSyntax::missing(SyntaxKind::ExprList);
   }
   return m_data->replaceChild<SequenceExprSyntax>(rawElements, Cursor::Elements);
}

SequenceExprSyntax SequenceExprSyntax::addElement(ExprSyntax expr)
{
   RefCountPtr<RawSyntax> elements = getRaw()->getChild(Cursor::Elements);
   if (elements) {
      elements = elements->append(expr.getRaw());
   } else {
      elements = RawSyntax::make(SyntaxKind::ExprList, {expr.getRaw()}, SourcePresence::Present);
   }
   return m_data->replaceChild<SequenceExprSyntax>(elements, Cursor::Elements);
}

void PrefixOperatorExprSyntax::validate()
{
#ifdef POLAR_DEBUG_BUILD
   RefCountPtr<RawSyntax> raw = m_data->getRaw();
   if (isMissing()) {
      return;
   }
   assert(raw->getLayout().size() == PrefixOperatorExprSyntax::CHILDREN_COUNT);
#endif
}

std::optional<TokenSyntax> PrefixOperatorExprSyntax::getOperatorToken()
{
   RefCountPtr<SyntaxData> operatorTokenData = m_data->getChild(Cursor::OperatorToken);
   if (!operatorTokenData) {
      return std::nullopt;
   }
   return TokenSyntax{m_root, operatorTokenData.get()};
}

ExprSyntax PrefixOperatorExprSyntax::getExpr()
{
   return ExprSyntax{m_root, m_data->getChild(Cursor::Expr).get()};
}

PrefixOperatorExprSyntax PrefixOperatorExprSyntax::withOperatorToken(std::optional<TokenSyntax> operatorToken)
{
   RefCountPtr<RawSyntax> rawOperatorToken;
   if (operatorToken.has_value()) {
      rawOperatorToken = operatorToken->getRaw();
   } else {
      rawOperatorToken = nullptr;
   }
   return m_data->replaceChild<PrefixOperatorExprSyntax>(rawOperatorToken, Cursor::OperatorToken);
}

PrefixOperatorExprSyntax PrefixOperatorExprSyntax::withExpr(std::optional<TokenSyntax> expr)
{
   RefCountPtr<RawSyntax> rawExpr;
   if (expr.has_value()) {
      rawExpr = expr->getRaw();
   } else {
      rawExpr = RawSyntax::missing(SyntaxKind::Expr);
   }
   return m_data->replaceChild<PrefixOperatorExprSyntax>(rawExpr, Cursor::Expr);
}

void PostfixOperatorExprSyntax::validate()
{
#ifdef POLAR_DEBUG_BUILD
   RefCountPtr<RawSyntax> raw = m_data->getRaw();
   if (isMissing()) {
      return;
   }
   assert(raw->getLayout().size() == PostfixOperatorExprSyntax::CHILDREN_COUNT);
#endif
}

ExprSyntax PostfixOperatorExprSyntax::getExpr()
{
   return ExprSyntax{m_root, m_data->getChild(Cursor::Expr).get()};
}

TokenSyntax PostfixOperatorExprSyntax::getOperatorToken()
{
   return TokenSyntax{m_root, m_data->getChild(Cursor::OperatorToken).get()};
}

PostfixOperatorExprSyntax PostfixOperatorExprSyntax::withExpr(std::optional<ExprSyntax> expr)
{
   RefCountPtr<RawSyntax> rawExpr;
   if (expr.has_value()) {
      rawExpr = expr->getRaw();
   } else {
      rawExpr = RawSyntax::missing(SyntaxKind::Expr);
   }
   return m_data->replaceChild<PostfixOperatorExprSyntax>(rawExpr, Cursor::Expr);
}

PostfixOperatorExprSyntax PostfixOperatorExprSyntax::withOperatorToken(std::optional<TokenSyntax> operatorToken)
{
   RefCountPtr<RawSyntax> rawOperatorToken;
   if (operatorToken.has_value()) {
      rawOperatorToken = operatorToken->getRaw();
   } else {
      rawOperatorToken = RawSyntax::missing(TokenKindType::T_POSTFIX_OPERATOR,
                                            OwnedString::makeUnowned(""));
   }
   return m_data->replaceChild<PostfixOperatorExprSyntax>(rawOperatorToken, Cursor::OperatorToken);
}

void BinaryOperatorExprSyntax::validate()
{
#ifdef POLAR_DEBUG_BUILD
   RefCountPtr<RawSyntax> raw = m_data->getRaw();
   if (isMissing()) {
      return;
   }
   assert(raw->getLayout().size() == BinaryOperatorExprSyntax::CHILDREN_COUNT);
#endif
}

TokenSyntax BinaryOperatorExprSyntax::getOperatorToken()
{
   return TokenSyntax{m_root, m_data->getChild(Cursor::OperatorToken).get()};
}

BinaryOperatorExprSyntax BinaryOperatorExprSyntax::withOperatorToken(std::optional<TokenSyntax> operatorToken)
{
   RefCountPtr<RawSyntax> rawOperatorToken;
   if (operatorToken.has_value()) {
      rawOperatorToken = operatorToken->getRaw();
   } else {
      rawOperatorToken = RawSyntax::missing(TokenKindType::T_BINARY_OPERATOR,
                                            OwnedString::makeUnowned(""));
   }
   return m_data->replaceChild<BinaryOperatorExprSyntax>(rawOperatorToken, TokenKindType::T_BINARY_OPERATOR);
}

///
/// LexicalVarItemSyntax
///
void LexicalVarItemSyntax::validate()
{
#ifdef POLAR_DEBUG_BUILD
   RefCountPtr<RawSyntax> raw = m_data->getRaw();
   if (isMissing()) {
      return;
   }
   assert(raw->getLayout().getSize() == LexicalVarItemSyntax::CHILDREN_COUNT);
   syntax_assert_child_token(raw, ReferenceToken, std::set{TokenKindType::T_AMPERSAND});
   syntax_assert_child_token(raw, Variable, std::set{TokenKindType::T_VARIABLE});
#endif
}

std::optional<TokenSyntax> LexicalVarItemSyntax::getReferenceToken()
{
   RefCountPtr<SyntaxData> refData = m_data->getChild(Cursor::ReferenceToken);
   if (!refData) {
      return std::nullopt;
   }
   return TokenSyntax {m_root, refData.get()};
}

TokenSyntax LexicalVarItemSyntax::getVariable()
{
   return TokenSyntax {m_root, m_data->getChild(Cursor::Variable).get()};
}

LexicalVarItemSyntax LexicalVarItemSyntax::withReferenceToken(std::optional<TokenSyntax> referenceToken)
{
   RefCountPtr<RawSyntax> referenceTokenRaw;
   if (referenceToken.has_value()) {
      referenceTokenRaw = referenceToken->getRaw();
   } else {
      referenceTokenRaw = nullptr;
   }
   return m_data->replaceChild<LexicalVarItemSyntax>(referenceTokenRaw, Cursor::ReferenceToken);
}

LexicalVarItemSyntax LexicalVarItemSyntax::withVariable(std::optional<TokenSyntax> variable)
{
   RefCountPtr<RawSyntax> variableRaw;
   if (variable.has_value()) {
      variableRaw = variable->getRaw();
   } else {
      variableRaw = RawSyntax::missing(TokenKindType::T_VARIABLE,
                                       OwnedString::makeUnowned(get_token_text(TokenKindType::T_VARIABLE)));
   }
   return m_data->replaceChild<LexicalVarItemSyntax>(variableRaw, Cursor::Variable);
}

} // polar::syntax
