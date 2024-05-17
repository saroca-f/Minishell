/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: saroca-f <saroca-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 18:17:58 by schamizo          #+#    #+#             */
/*   Updated: 2024/05/17 14:33:00 by saroca-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../bash.h"

t_token	*get_next_token(t_input *input);
t_ast	*ft_expr(t_input *input);
t_ast	*ft_outfile(t_input *input, t_idenlst **list);
t_ast	*ft_factor(t_input *input, t_idenlst **list);

void	ft_add_identifier_front(t_idenlst **lst, t_idenlst *new)
{
	if (new && lst)
	{
		new->next = *lst;
		*lst = new;
	}
}

void	ft_lstadd_identifier(t_idenlst **lst, t_idenlst *new)
{
	t_idenlst	*last;

	last = *lst;
	if (!last)
		*lst = new;
	else
	{
		while (last->next)
			last = last->next;
		last->next = new;
		new->prev = last;
	}
}

t_idenlst	*ft_lstnew_identifier(t_token *token)
{
	t_idenlst	*new;

	new = NULL;
	new = malloc(sizeof(t_idenlst));
	if (!new)
		return (NULL);
	if (new)
	{
		new->token = token;
		new->next = NULL;
		new->prev = NULL;
	}
	return (new);
}

/*void	ft_add_identifier(t_input *input)
{
	t_idenlst	*new_node;

	new_node = ft_lstnew_identifier(input->current_token);
	ft_lstadd_identifier(&input->list, new_node);
}*/

void	ft_eat(t_input *input, t_tokentype type)
{
	if (input->current_token->type == type)
		input->current_token = get_next_token(input);
}

t_ast	*ft_assignment(t_input *input, t_idenlst **list)
{
	t_token	*token;
	t_ast	*ast_node;

	ast_node = NULL;
	token = input->current_token;
	if (token->type == T_ASSING)
	{
		ft_eat(input, T_ASSING);
		ast_node = new_b_node(NULL, token, ft_factor(input, list));
	}
	else if (token->type == T_O_PARENT)
	{
		ast_node = ft_expr(input);
		ft_eat(input, T_C_PARENT);
		return (ast_node);
	}
	return (ast_node);
}



/*t_ast	*ft_factor(t_input *input)
{
	t_token	*token;
	t_ast	*ast_node;

	ast_node = NULL;
	token = input->current_token;
	if (token->type == T_IDENTIFIER && !check_equal(token->value))
	{
		ft_eat(input, T_IDENTIFIER);
		ast_node = new_b_node(ft_factor(input), token, ft_outfile(input));
		return (ast_node);
	}
	else if (token->type == T_IDENTIFIER && check_equal(token->value))
	{
		ft_eat(input, T_IDENTIFIER);
		ast_node = new_b_node(NULL, token, ft_outfile(input));
	}
	else if (token->type == T_O_PARENT)
	{
		ast_node = ft_expr(input);
		ft_eat(input, T_C_PARENT);
		return (ast_node);
	}
	return (ast_node);
}*/

t_ast	*ft_factor(t_input *input, t_idenlst **list)
{
	t_token	*token;
	t_ast	*ast_node;
	t_idenlst	*new_node;

	ast_node = ft_assignment(input, list);
	token = input->current_token;
	if (token->type == T_IDENTIFIER)
	{
		/*ft_eat(input, T_IDENTIFIER);
		ast_node = new_b_node(ft_factor(input), token, ast_node);*/
		new_node = ft_lstnew_identifier(input->current_token);
		printf("%s\n", new_node->token->value);
		ft_lstadd_identifier(list, new_node);
		ft_eat(input, T_IDENTIFIER);
		ast_node = ft_outfile(input, list);
		/*if (ast_node)
			ast_node = new_b_node(ft_factor(input), token, ast_node);
		else
			ast_node = new_b_node(ft_factor(input), token, ft_factor(input));*/
	}
	return (ast_node);
}

t_ast	*ft_term(t_input *input, t_idenlst **list)
{
	t_ast	*ast;
	t_token	*token;
	t_token	*token2;

	ast = ft_factor(input, list);
	token = input->current_token;
	if (input->current_token->type == T_LESS
		|| input->current_token->type == T_DLESS)
	{
		ft_eat_aux(input);
		token2 = input->current_token;
		ft_eat(input, T_IDENTIFIER);
		if (ast)
			ast = new_b_node(new_i_node(token2), token, ast);
		else
			ast = new_b_node(new_i_node(token2), token, ft_outfile(input, list));
	}
	return (ast);
}

t_ast	*ft_outfile(t_input *input, t_idenlst **list)
{
	t_ast	*ast;
	t_token	*token;
	t_token	*token2;

	ast = ft_term(input, list);
	token = input->current_token;
	if (input->current_token->type == T_GREAT
		|| input->current_token->type == T_DGREAT)
	{
		ft_eat_aux(input);
		token2 = input->current_token;
		ft_eat(input, T_IDENTIFIER);
		if (ast)
			ast = new_b_node(new_i_node(token2), token, ast);
		else
			ast = new_b_node(new_i_node(token2), token, ft_outfile(input, list));
	}
	return (ast);
}

t_ast	*ft_expr(t_input *input)
{
	t_ast	*ast;
	t_ast	*ast2;
	//t_ast	*ast3;
	t_token	*token;
	t_idenlst *list;

	list = NULL;
	ast2 = NULL;
	input->current_token = get_next_token(input);
	ast = ft_outfile(input, &list);
	token = input->current_token;
	if (input->current_token->type == T_PIPE)
	{
		ast = new_b_node(ast, token, ft_expr(input));
	}
	if (list)
	{
		printf("ENTRA AQUI 3\n");
		ast2 = iden_node(list);
		print_ast(ast2);
	}
	if (ast2 && ast && ast->right == NULL)
	{
		printf("ENTRA AQUI\n");
		ast->right = ast2;
	}
	if (ast2 && ast && ast->right != NULL && ast->token->type != T_PIPE)
	{
		printf("ENTRA AQUI 2\n");
		//ast3 = ast->right;
		//ast->right = ast2;
		ast2->right = ast;
		return (ast2);
	}
	return (ast);
}
